/*****************************************************************************
*  C++ 图像读取
*  Copyright (C) 2020 1158292387@qq.com
*
*  @file     
*  @brief    对文件的简单概述
*  Details.
*
*  @author   Leeme
*  @email    1158292387@qq.com
*  @version  1.0.0.1(版本)
*  @date     2020.11.21
*  @license  GNU General Public License (GPL)
*
*----------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2020/10/30 | 1.0.0.1   | Leeme          | Create file
*----------------------------------------------------------------------------
*
*****************************************************************************/
#ifndef IMAGEREADER_H
#define IMAGEREADER_H

#include <string>
#include <iostream>
#include "public.h"

/**
* @brief 简单概述
* @brief 图像读取基类
*/
class IMAGEALG_EXPORT reader
{
public:
	reader();
	virtual ~reader();

	virtual bool Read(std::string) = 0;
	virtual bool CloneImage(CImage&);
	const CImage&	GetImage() { return m_Image; }

protected:
	bool GetImageStream(const char* path, uint8*& pBuf);
	CImage	m_Image;
};

/**
* @brief 简单概述
* @brief BMP图像读取类
*/
class IMAGEALG_EXPORT BmpReader :public reader
{
public:
	BmpReader();
	~BmpReader();

	/**
	* @brief 简单概述
	* @brief 读取图像
	*/
	bool Read(std::string path);

	/**
	* @brief 简单概述
	* @brief 读取图像
	*/
	bool ReadData(std::string path);

	// return bits per pixel, 8 means grayscale, 24 means RGB color, 32 means RGBA
	inline int getBitCount() const { return bitCount; }
	inline int getDataSize() const { return dataSize; }
	inline const char* getError() const { return errorMessage.c_str(); }

	///////////////////////////////////////////////////////////////////////////////
	// clear out the exsiting values
	///////////////////////////////////////////////////////////////////////////////
	void init()
	{
		width = height = bitCount = dataSize = 0;
		errorMessage = "No error.";
	}

private:

	/**
	* @brief 简单概述
	* @brief 解析RGBA图像数据
	*/
	bool ReadMutiColorBuffer(uint8*& imageBuffer, const BmpFileHeader& FileHr, const BmpInfoHeader& InfoHr);
	bool GetBmpHeader(const uint8* pBuf, BmpFileHeader& FileHr, BmpInfoHeader& InfoHr);

	bool decodeRLE8(const uint8 *encData, uint8 *data);              // decode BMP 8-bit RLE to uncompressed
	void flipImage(uint8 *data, int width, int height, int channelCount);    // flip the vertical orientation
	void swapRedBlue(uint8 *data, int dataSize, int channelCount);           // swap the position of red and blue components
	int  getColorCount(const uint8 *data, int dataSize);                     // get the number of colors used in 8-bit grayscale image
	void buildGrayScalePalette(uint8 *palette, int paletteSize);

	// member variables
	int width, height;
	int bitCount, dataSize;
	std::string errorMessage;
};

/**
* @brief 简单概述
* @brief PNG图像读取类
*/
class IMAGEALG_EXPORT PngReader :public reader
{
public:
	PngReader();
	~PngReader();
	bool Read(std::string);
};

/**
* @brief 简单概述
* @brief JPEG图像读取类
*/
class IMAGEALG_EXPORT JpegReader :public reader
{
public:
	JpegReader();
	~JpegReader();
	bool Read(std::string);
};

/**
* @brief 简单概述
* @brief 图像智能读取类
*/
class IMAGEALG_EXPORT ImageReader
{
public:
    ImageReader();
    ~ImageReader();

	/**
	* @brief 简单概述
	* @brief 读取指定类型的图像.
	*/
	bool ReadImageFile(const char* path, const ImageFileType& type, CImage& image);

    /**
    * @brief 简单概述
    * @brief 拷贝图像
    */
    bool CloneImage(CImage& img);

private:
	reader*  m_pReader;
};

#endif