#include "ImageReader.h"
#include "png.h"
#include "jpeglib.h"
#include <setjmp.h>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

reader::reader()
{

}
reader::~reader()
{
	if (m_Image.pBuffer != NULL)
	{
		delete[] m_Image.pBuffer;
		m_Image.pBuffer = NULL;
	}
}

/**
* @brief 简单概述
* @brief 读取图像字节流
*/
bool reader::GetImageStream(const char* path, uint8*& pBuf)
{
	if (path == NULL || *path == '\0')
		return false;

	FILE* file = fopen(path, "rb");
	if (file)
	{
		uint8* imageBuffer = nullptr;
		fseek(file, 0, SEEK_END);
		int length = ftell(file);
		if (length > 0)
		{
			rewind(file);
			imageBuffer = new uint8[length + 1];
			fread(imageBuffer, sizeof(uint8), length, file);
			imageBuffer[length] = '\0';
			pBuf = imageBuffer;
		}
		fclose(file);
		return true;
	}
	return false;
}

/**
* @brief 简单概述
* @brief 克隆图像
*/
bool reader::CloneImage(CImage& img)
{
	if (0 == m_Image.width || 0 == m_Image.height || m_Image.pBuffer == NULL)
		return false;
	memcpy(&img, &m_Image, sizeof(CImage));
	img.pBuffer = new uint8[m_Image.iBufSize];
	memcpy(img.pBuffer, m_Image.pBuffer, m_Image.iBufSize);
	return true;
}

BmpReader::BmpReader()
{
	
}

BmpReader::~BmpReader()
{
	if (m_Image.pBuffer != NULL)
	{
		delete[] m_Image.pBuffer;
		m_Image.pBuffer = NULL;
	}
}

/**
* @brief 简单概述
* @brief 读取图像
*/
bool BmpReader::ReadData(std::string fileName)
{
	if (fileName.empty())
		return false;

	this->init();   // clear out all values

					// check NULL pointer
	if (!fileName.c_str())
	{
		errorMessage = "File name is not defined (NULL pointer).";
		return false;
	}

	// open a BMP file as binary mode
	std::ifstream inFile;
	inFile.open(fileName, std::ios::binary);         // binary mode
	if (!inFile.good())
	{
		errorMessage = "Failed to open a BMP file to read.";
		return false;            // exit if failed
	}

	// list of entries in BMP header
	char id[2];             // magic identifier "BM" (2 bytes)
	int fileSize;           // file size in bytes (4)
	short reserved1;        // reserved 1 (2)
	short reserved2;        // reserved 2 (2)
	int dataOffset;         // starting offset of bitmap data (4)
	int infoHeaderSize;     // info header size (4)
	int width;              // image width (4)
	int height;             // image height (4)
	short planeCount;       // # of planes (2)
	short bitCount;         // # of bits per pixel (2)
	int compression;        // compression mode (4)
	int dataSizeWithPaddings; // bitmap data size with paddings in bytes (4)
							  //int xResolution;        // horizontal pixels per metre (4)
							  //int yResolution;        // vertical pixels per metre (4)
							  //int colorCount;         // # of colours used (4)
							  //int importantColorCount;// # of important colours (4)
	uint8 *data;      //data with default BGR order

							  // read BMP header infos
	inFile.read(id, 2);                         // should be "BM"
	inFile.read((char*)&fileSize, 4);           // should be same as file size
	inFile.read((char*)&reserved1, 2);          // should be 0
	inFile.read((char*)&reserved2, 2);          // should be 0
	inFile.read((char*)&dataOffset, 4);
	inFile.read((char*)&infoHeaderSize, 4);     // should be 40
	inFile.read((char*)&width, 4);
	inFile.read((char*)&height, 4);
	inFile.read((char*)&planeCount, 2);         // should be 1
	inFile.read((char*)&bitCount, 2);           // 1, 4, 8, 24, or 32
	inFile.read((char*)&compression, 4);        // 0(uncompressed), 1(8-bit RLE), 2(4-bit RLE), 3(RGB with mask)
	inFile.read((char*)&dataSizeWithPaddings, 4);
	//inFile.read((char*)&xResolution, 4);
	//inFile.read((char*)&yResolution, 4);
	//inFile.read((char*)&colorCount, 4);
	//inFile.read((char*)&importantColorCount, 4);

	// check magic ID, "BM"
	if (id[0] != 'B' && id[1] != 'M')
	{
		// it is not BMP file, close the opened file and exit
		inFile.close();
		errorMessage = "Magic ID is invalid.";
		return false;
	}

	// it supports only 8-bit grayscale, 24-bit BGR or 32-bit BGRA
	if (bitCount < 8)
	{
		inFile.close();
		errorMessage = "Unsupported format.";
		return false;
	}

	// it supports only uncompressed and 8-bit RLE compressed format
	if (compression > 1)
	{
		inFile.close();
		errorMessage = "Unsupported compression mode.";
		return false;
	}

	// do not trust the file size in header, recalculate it
	inFile.seekg(0, std::ios::end);
	fileSize = inFile.tellg();

	// compute the number of paddings
	// In BMP, each scanline must be divisible evenly by 4.
	// If not divisible by 4, then each line adds
	// extra paddings. So it can be divided evenly by 4.
	int paddings = (4 - ((width * bitCount / 8) % 4)) % 4;

	// compute data size without paddings
	int dataSize = width * height * bitCount / 8;

	// recompute data size with paddings (do not trust the data size in header)
	dataSizeWithPaddings = fileSize - dataOffset;   // it maybe greater than "dataSize+(height*paddings)" because 4-byte boundary for file size

													// now it is ready to store info and image data
	this->width = width;
	this->height = height;
	this->bitCount = bitCount;
	this->dataSize = dataSize;

	m_Image.width = width;
	m_Image.height = height;
	m_Image.channel = bitCount / 8;
	m_Image.iBufSize = dataSize;

	if (bitCount == 8)
		m_Image.format = IMAGE_GREY;
	else if (bitCount == 24)
		m_Image.format = IMAGE_RGB;
	else if (bitCount == 32)
		m_Image.format = IMAGE_RGBA;

	// allocate data arrays
	// add extra bytes for paddings if width is not divisible by 4
	data = new uint8[dataSizeWithPaddings];
	m_Image.pBuffer = new uint8[dataSize];

	if (compression == 0)                    // uncompressed
	{
		inFile.seekg(dataOffset, std::ios::beg); // move cursor to the starting position of data
		inFile.read((char*)data, dataSizeWithPaddings);
	}
	else if (compression == 1)               // 8-bit RLE(Run Length Encode) compressed
	{
		// get length of encoded data
		int size = fileSize - dataOffset;

		// allocate tmp array to store the encoded data
		uint8 *encData = new uint8[size];

		// read data from file
		inFile.seekg(dataOffset, std::ios::beg);
		inFile.read((char*)encData, size);

		// decode RLE into image data buffer
		decodeRLE8(encData, data);

		// deallocate encoded data buffer after decoding
		delete[] encData;
	}

	// close it after reading
	inFile.close();

	// we don't need paddings, trim paddings from each line
	// Note that there is no padding in RLE compressed data
	if (compression == 0 && paddings > 0)
	{
		int lineWidth = width * bitCount / 8;

		// copy line by line
		for (int i = 1; i < height; ++i)
		{
			memcpy(&data[i*(lineWidth + paddings)], &data[i*(lineWidth + paddings)], (lineWidth + paddings));
		}
	}

	// BMP is bottom-to-top orientation by default, flip image vertically
	// But if the height is negative value, then it is top-to-bottom orientation.
	if (height > 0)
		flipImage(data, width, height, bitCount / 8);

	// the colour components order of BMP image is BGR
	// convert image data to RGB order for convenience
	memcpy(m_Image.pBuffer, data, dataSize);    // copy data to dataRGB first
	if (bitCount == 24 || bitCount == 32)
		swapRedBlue(m_Image.pBuffer, dataSize, bitCount / 8);

	delete[] data;
	data = NULL;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// decode 8-bit RLE data into uncompressed data
// This routine need 2 pointers: the pointer to the encoded input data and
// the pointer to the decoded output data. The length of input array is not
// necessary because the last 2 bytes of input data must be 00 and 01, which
// tells the end of data. So it can stop decoding process.
//
// BMP uses 2-value RLE scheme: the first value contains a count of the number
// of pixels in the run, and the second value contains the value of the pixel
// repeated. For example, 0x3 0xFF means 0xFF 0xFF 0xFF.
//
// If the first value is 0x00, then it is unencoded run mode and a pixel is not
// repeated any more. In unencode run mode, the second value is the the number
// of unencoded pixel values that follow. If the number of pixels is odd, then
// a 0x00 padding value also follows.
// 1st  2nd  EncodedValue  DecodedValue
// ===  ===  ============  ============
//  00   03  FF FE FD 00   FF FE FD
//  00   04  11 12 13 14   11 12 13 14
//
// The second value of unencoded run mode must be greater than and equal to 3.
// If the second value is less than 3, then it specifies special positioning
// operations and does not decode any data themselves.
// 1st  2nd  Meaning
// ===  ===  ==============================================
//  00   00  End of Scanline, Decode new data at the next line
//  00   01  End of Bitmap data, Stop decoding data here
//  00   02  Delta Offset, Move the cursor hori and vert direction
//
// Delta Offset operation requires 4-byte in size: the first and second should
// be 00 and 02, and the third byte is the number of pixels forward in the
// same scanline and the fourth byte is the number of rows to move. For
// example, 00 02 03 04 means move the cursor 3 pixels right, and 4 pixels
// upward. (Note that BMP is bottom-to-top orientation.)
///////////////////////////////////////////////////////////////////////////////
bool BmpReader::decodeRLE8(const uint8 *encData, uint8 *outData)
{
	// check NULL pointer
	if (!encData || !outData)
		return false;

	uint8 first, second;
	int i;
	bool stop = false;

	// start decoding, stop when it reaches at the end of decoded data
	while (!stop)
	{
		// grab 2 bytes at the current position
		first = *encData++;
		second = *encData++;

		if (first)                   // encoded run mode
		{
			for (i = 0; i < first; ++i)
				*outData++ = second;
		}
		else
		{
			if (second == 1)         // reached the end of bitmap
				stop = true;        // must stop decoding

			else if (second == 2)    // delta mark
				encData += 2;       // do nothing, but move the cursor 2 more bytes

			else                    // unencoded run mode (second >= 3)
			{
				for (i = 0; i < second; ++i)
					*outData++ = *encData++;

				if (second % 2)      // if it is odd number, then there is a padding 0. ignore it
					encData++;
			}
		}
	}

	return true;
}



///////////////////////////////////////////////////////////////////////////////
// BMP is bottom-to-top orientation. Flip the image vertically, so the image
// can be rendered from top to bottom orientation
///////////////////////////////////////////////////////////////////////////////
void BmpReader::flipImage(uint8 *data, int width, int height, int channelCount)
{
	if (!data) return;

	int lineSize = width * channelCount;
	uint8* tmp = new uint8[lineSize];
	int half = height / 2;

	int line1 = 0;                          // first line
	int line2 = (height - 1) * lineSize;    // last line

											// scan only half of height
	for (int i = 0; i < half; ++i)
	{
		// copy line by line
		memcpy(tmp, &data[line1], lineSize);
		memcpy(&data[line1], &data[line2], lineSize);
		memcpy(&data[line2], tmp, lineSize);

		// move to next line
		line1 += lineSize;
		line2 -= lineSize;
	}

	// deallocate temp arrays
	delete[] tmp;
}



///////////////////////////////////////////////////////////////////////////////
// swap the position of the 1st and 3rd color components (RGB <-> BGR)
///////////////////////////////////////////////////////////////////////////////
void BmpReader::swapRedBlue(uint8 *data, int dataSize, int channelCount)
{
	if (!data) return;
	if (channelCount < 3) return;            // must be 3 or 4
	if (dataSize % channelCount) return;     // must be divisible by the number of channels

	uint8 tmp;
	int i;

	// swap the position of red and blue components
	for (i = 0; i < dataSize; i += channelCount)
	{
		tmp = data[i];
		data[i] = data[i + 2];
		data[i + 2] = tmp;
	}
}

///////////////////////////////////////////////////////////////////////////////
// compute the number of used colors in the 8-bit grayscale image
///////////////////////////////////////////////////////////////////////////////
int BmpReader::getColorCount(const uint8* data, int dataSize)
{
	if (!data) return 0;

	const int MAX_COLOR = 256;  // max number of colors in 8-bit grayscale
	int i;
	int colorCount = 0;
	uint32 colors[MAX_COLOR];

	// clear all to 0s
	memset((void*)colors, 0, MAX_COLOR);

	// increment at the same index
	for (i = 0; i < dataSize; ++i)
		colors[data[i]]++;

	// count backward the number of color used in this data
	colorCount = MAX_COLOR;
	for (i = 0; i < MAX_COLOR; ++i)
	{
		if (colors[i] == 0)
			colorCount--;
	}

	return colorCount;
}

///////////////////////////////////////////////////////////////////////////////
// build palette for 8-bit grayscale image
// Each component(B,G,R,A) of palette will have the same value as data value
// because it is grayscale.
///////////////////////////////////////////////////////////////////////////////
void BmpReader::buildGrayScalePalette(uint8* palette, int paletteSize)
{
	if (!palette) return;

	// fill B, G, R, with same value and A is 0
	int i, j;
	for (i = 0, j = 0; i < paletteSize; i += 4, j++)
	{
		palette[i] = palette[i + 1] = palette[i + 2] = (uint8)j;
		palette[i + 3] = (uint8)0;
	}
}

/**
* @brief 简单概述
* @brief 读取图像
*/
bool BmpReader::Read(std::string path)
{
	uint8* pBuf;
	BmpFileHeader FileHr;
	BmpInfoHeader InfoHr;
	if (this->GetImageStream(path.c_str(), pBuf) && GetBmpHeader(pBuf, FileHr, InfoHr))
	{
		return ReadMutiColorBuffer(pBuf, FileHr, InfoHr);
	}
	else
		return false;
}

/**
* @brief 简单概述
* @brief 解析图像数据头部信息
*/
bool BmpReader::GetBmpHeader(const uint8* pBuf, BmpFileHeader& FileHr, BmpInfoHeader& InfoHr)
{
	if (pBuf == NULL)
		return false;
	const uint8* pBuffer = pBuf;
	if (0x4D42 == *((uint16*)pBuffer))
	{
		pBuffer += BYTE_2;
		memcpy(&FileHr, pBuffer, sizeof(BmpFileHeader));
		pBuffer += sizeof(BmpFileHeader);
		memcpy(&InfoHr, pBuffer, sizeof(BmpInfoHeader));
		return true;
	}
	else
		return false;
}

/**
* @brief 简单概述
* @brief 解析RGBA图像数据
*/
bool BmpReader::ReadMutiColorBuffer(uint8*& imageBuffer, const BmpFileHeader& FileHr, const BmpInfoHeader& InfoHr)
{
	m_Image.width = InfoHr.biWidth;
	m_Image.height = InfoHr.biHeight;
	if (InfoHr.biBitCount == 24)
	{
        m_Image.channel = 3;
		m_Image.format = IMAGE_RGB;
	}
	else if (InfoHr.biBitCount == 32)
	{
        m_Image.channel = 4;
		m_Image.format = IMAGE_RGBA;
	}
	else if (InfoHr.biBitCount == 8)
	{
		m_Image.channel = 1;
		m_Image.format = IMAGE_GREY;
	}
	else
		return false;

	// compute the number of paddings
	// In BMP, each scanline must be divisible evenly by 4.
	// If not divisible by 4, then each line adds
	// extra paddings. So it can be divided evenly by 4.
	int paddings = (4-((m_Image.width * InfoHr.biBitCount /8)%4))%4;
	
	uint16 PerLine = m_Image.width*m_Image.channel+paddings;
	unsigned long FileBufSize = PerLine * m_Image.height;
	m_Image.iBufSize = FileBufSize;

	uint8* pData = imageBuffer + FileHr.bfOffBits;
	m_Image.pBuffer = new uint8[m_Image.iBufSize];
	memcpy(m_Image.pBuffer, pData, m_Image.iBufSize);

	// 用备份的指针遍历.
	pData = m_Image.pBuffer;
	for (unsigned long m = 0; m < m_Image.height; m ++)
	{
		for (unsigned long i = 0; i < PerLine-paddings; i+= m_Image.channel)
		{
			// 交换R-B两个通道.
			uint8 temp = pData[i];
			pData[i] = pData[i+2];
			pData[i+2] = temp;
		}
		pData += PerLine;
	}
	delete[]imageBuffer;

	// 高度为正数，需要做垂直翻转.
	if (m_Image.height > 0)
	{
		uint8* tmp = new uint8[PerLine];

		int line_f = 0;									// first line
		int line_l = (m_Image.height - 1) * PerLine;	// last line
		int half = m_Image.height / 2;
		pData = m_Image.pBuffer;
		
		// scan only half of height
		for (int i = 0; i < half; ++i)
		{
			// copy line by line
			memcpy(tmp, &pData[line_f], PerLine);
			memcpy(&pData[line_f], &pData[line_l], PerLine);
			memcpy(&pData[line_l], tmp, PerLine);

			// move to next line
			line_f += PerLine;
			line_l -= PerLine;
		}
		delete[] tmp;
	}
	return true;
}

void abort_(const char * s, ...)
{
	va_list args;
	va_start(args, s);
	vfprintf(stderr, s, args);
	fprintf(stderr, "\n");
	va_end(args);
	abort();
}

PngReader::PngReader()
{

}

PngReader::~PngReader()
{
	if (m_Image.pBuffer != NULL)
	{
		delete[] m_Image.pBuffer;
		m_Image.pBuffer = NULL;
	}
}

bool PngReader::Read(std::string path)
{
	// unfortunately, we need to break down to the C-code level here, since
	// libpng is written in C itself

	// we need to open the file in binary mode
	FILE * input = fopen(path.c_str(), "rb");
	if (!input)
	{
		return false;
	}

	// read in the header (max size of 8), use it to validate this as a PNG file
	png_byte header[8];
	fread(header, 1, 8, input);
	if (png_sig_cmp(header, 0, 8))
	{
		fclose(input);
		return false;
	}

	// set up libpng structs for reading info
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL); 
	if (!png_ptr)
	{
		fclose(input);
		return false;
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		fclose(input);
		return false;
	}

	// set error handling to not abort the entire program
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(input);
		return false;
	}

	// initialize png reading
	png_init_io(png_ptr, input);
	// let it know we've already read the first 8 bytes
	png_set_sig_bytes(png_ptr, 8);

	// read in the basic image info
	png_read_info(png_ptr, info_ptr);

	// convert to 8 bits
	png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	if (bit_depth == 16)
		png_set_strip_16(png_ptr);

	// verify this is in RGBA format, and if not, convert it to RGBA
	png_byte color_type = png_get_color_type(png_ptr, info_ptr);
	if (color_type != PNG_COLOR_TYPE_RGBA && color_type != PNG_COLOR_TYPE_RGB)
	{
		if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		{
			if (bit_depth < 8)
				png_set_expand(png_ptr);
			png_set_gray_to_rgb(png_ptr);
		}
		if (color_type == PNG_COLOR_TYPE_PALETTE)
			png_set_palette_to_rgb(png_ptr);
	}
	// convert tRNS to alpha channel
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);

	m_Image.width = png_get_image_width(png_ptr, info_ptr);
	m_Image.height = png_get_image_height(png_ptr, info_ptr);
	m_Image.channel = png_get_channels(png_ptr, info_ptr);
	int bitCount = png_get_bit_depth(png_ptr, info_ptr);

	if (color_type == PNG_COLOR_TYPE_GRAY)
		m_Image.format = IMAGE_GREY;
	else if (color_type == PNG_COLOR_TYPE_RGB)
		m_Image.format = IMAGE_RGB;
	else if (color_type == PNG_COLOR_TYPE_RGBA)
		m_Image.format = IMAGE_RGBA;

	png_read_update_info(png_ptr, info_ptr);

	// begin reading in the image
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(input);
		return false;
	}

	// compute the number of paddings
	// In BMP, each scanline must be divisible evenly by 4.
	// If not divisible by 4, then each line adds
	// extra paddings. So it can be divided evenly by 4.
	int paddings = (4 - ((m_Image.width * bitCount / 8) % 4)) % 4;
	int bpr = png_get_rowbytes(png_ptr, info_ptr); // number of bytes in a row

	// initialie our image storage
	m_Image.iBufSize = m_Image.width*m_Image.height * m_Image.channel + paddings*m_Image.height;
	m_Image.pBuffer = new uint8[m_Image.iBufSize];
	png_byte * row = new png_byte[bpr];
	for (int y = 0; y < m_Image.height; y++)
	{
		png_read_row(png_ptr, row, NULL);
		memcpy(m_Image.pBuffer, row, m_Image.width * m_Image.channel+paddings);
		m_Image.pBuffer += (m_Image.width * m_Image.channel+paddings);
	}
	m_Image.pBuffer -= m_Image.iBufSize;

	// cleanup
	delete[] row;
	png_read_end(png_ptr, NULL);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(input);
	return true;
}

JpegReader::JpegReader()
{

}
JpegReader::~JpegReader()
{
	
}

bool JpegReader::Read(std::string path)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE *infile = fopen(path.c_str(), "rb");
	if (!infile) 
	{
		return false;
	}
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, infile);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);
	int row_stride = cinfo.output_width * cinfo.output_components;
	JSAMPARRAY buffer = (JSAMPARRAY)malloc(sizeof(JSAMPROW));
	buffer[0] = (JSAMPROW)malloc(row_stride * sizeof(JSAMPLE));

	m_Image.channel = cinfo.output_components;
	m_Image.height = cinfo.output_height;
	m_Image.width = cinfo.output_width;
	int paddings = (4 - ((m_Image.width*m_Image.channel) % 4)) % 4;
	uint32 linesize = m_Image.width*m_Image.channel + paddings;
	m_Image.iBufSize = linesize*m_Image.height;
	m_Image.pBuffer = new uint8[m_Image.iBufSize];
	memset(m_Image.pBuffer,0, m_Image.iBufSize);
	while (cinfo.output_scanline < cinfo.output_height) 
	{
		jpeg_read_scanlines(&cinfo, buffer, 1);
		memcpy(m_Image.pBuffer, buffer[0], row_stride * sizeof(JSAMPLE));
		m_Image.pBuffer += linesize;
		/*for (int i = 0; i < row_stride; i += cinfo.output_components)
		{
			// 处理像素数据
			JSAMPLE r = buffer[0][i];
			JSAMPLE g = buffer[0][i + 1];
			JSAMPLE b = buffer[0][i + 2];
		}*/
	}

	m_Image.pBuffer -= m_Image.iBufSize;
	m_Image.format = IMAGE_RGB;

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(infile);
	free(buffer[0]);
	free(buffer);

	return true;
}

ImageReader::ImageReader()
{
    m_pReader = NULL;
}

ImageReader::~ImageReader()
{
    if (m_pReader != NULL)
    {
        delete m_pReader;
        m_pReader = NULL;
    }
}

/**
* @brief 简单概述
* @brief 读取指定类型的图像.
*/
bool ImageReader::ReadImageFile(const char* path, const ImageFileType& type, CImage& image)
{
    if (m_pReader != NULL)
    {
        delete m_pReader;
        m_pReader = NULL;
    }

	bool ret = false;
	switch (type)
	{
		case FILE_TYPE_BMP:
		{
			m_pReader = new BmpReader();
			ret = m_pReader->Read(path);
			m_pReader->CloneImage(image);
		}
		break;
		case FILE_TYPE_PNG:
		{
			m_pReader = new PngReader();
			ret = m_pReader->Read(path);
			m_pReader->CloneImage(image);
		}
		break;
		case FILE_TYPE_JPG:
		{
			m_pReader = new JpegReader();
			ret = m_pReader->Read(path);
			m_pReader->CloneImage(image);
		}
		break;
		case FILE_TYPE_JPEG:
		{
			m_pReader = new JpegReader();
			ret = m_pReader->Read(path);
			m_pReader->CloneImage(image);
		}
		break;
		case FILE_TYPE_TIF:
		{

		}
		break;
		default:
			break;
	}
	return ret;
}

/**
* @brief 简单概述
* @brief 拷贝图像
*/
bool ImageReader::CloneImage(CImage& img)
{
    return m_pReader->CloneImage(img);
}