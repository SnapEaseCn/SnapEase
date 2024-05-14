#pragma once
#include <string>
#include <cstdint>

#define uint8  unsigned char
#define uint16 unsigned short
#define uint32 unsigned int

#define FILE_PATH_MAX_LEN	260
#define SOCKET_MSG_LEN		512

#if defined(COMMON_LIBRARY)
#define COMMON_EXPORT __declspec(dllexport)
#else
#define COMMON_EXPORT __declspec(dllimport)
#endif

#if defined(IMAGEALG_LIBRARY)
#  define IMAGEALG_EXPORT __declspec(dllexport)
#else
#  define IMAGEALG_EXPORT __declspec(dllimport)
#endif

#if defined(LIBIO_LIBRARY)
#  define LIBIO_EXPORT __declspec(dllexport)
#else
#  define LIBIO_EXPORT __declspec(dllimport)
#endif

#define BYTE_2				2
#define BYTE_4				4
#define MEASURE_TXT_HEIGHT	12
#define MEASURE_FONT_SIZE	8

#define CROSS_CENTER_SIZE  10
#define CROSS_LENGTH 2000
#define LOOK_UP_SIZE 80
#define LOOK_UP_SCALE_SIZE 200

#define IMAGE_ICO(ico) std::string(":/ico/icon/")+std::string(ico)+".png";

// 字节通常简写为“B”，而位通常简写为小写“b”，计算机存储器的大小通常用字节来表示.
// bit(b)->比特，Byte(B)->字节
// 1B=8b
// 1KB=1024B
// 1MB=1024KB
// 1GB=1024MB
// BMP图像文件头信息
typedef struct
{
	//uint16	bfType;				// 2B 文件类型,BM,BA,CT,CP,IC,PT
	uint32	bfSize;					// 4B 位图文件大小，单位为B
	uint16	bfReserved1;			// 2B 保留
	uint16	bfReserved2;			// 2B 保留
	uint32	bfOffBits;				// 4B 文件头开始到图像实际数据之间的偏移量，单位为B
}BmpFileHeader;

typedef struct
{
	uint32	biSize;					// 4B 数据结构所需要的字节大小
	long	biWidth;				// 4B 图像宽度
	long	biHeight;				// 4B 图像高度
	uint16	biPlane;				// 2B 颜色平面数
	uint16	biBitCount;				// 2B 图像位深度：1，4，8，16，24，32
	uint32	biCompression;			// 4B 图像数据压缩类型：0(BI_RGB,不压缩),1(BI_RLE8，8比特游程编码),2(BI_RLE4，4比特游程编码),3(BI_BITFIELDS，比特域),4(BI_JPEG),5(BI_PNG)
	uint32	biSizeImage;			// 4B 图像大小，用BI_RGB格式时，可设置为0.
	long	biXPelsPerMeter;		// 4B 水平分辨率
	long	biYPelsPerMeter;		// 4B 垂直分辨率
	uint32	biClrUsed;				// 4B 颜色索引数
	uint32	biClrImport;			// 4B 对图像显示有重要影响的颜色索引数，0表示都重要
}BmpInfoHeader;

/**
* @brief 简单概述
* @brief 图像文件类型
*/
enum ImageFileType
{
	FILE_TYPE_BMP = 0x0000,
	FILE_TYPE_PNG,
	FILE_TYPE_JPG,
	FILE_TYPE_JPEG,
	FILE_TYPE_TIF
};

/**
* @brief 简单概述
* @brief 图像格式
*/
enum ImageFormat
{
	IMAGE_BINARY = 0x0001,	// 二值图像
	IMAGE_GREY = 0x0008,	// 灰度图像
	IMAGE_RGB = 0x0018,	// 彩色RGB图像
	IMAGE_BGR = 0x0019,	// 彩色BGR图像
	IMAGE_RGBA = 0x0020,	// 彩色RGBA图像
	IMAGE_BGRA = 0x0021	// 彩色BGRA图像
};

/**
* @brief 简单概述
* @brief 重建类型
*/
enum BuildType
{
	BUILD_FRONT,
	BUILD_LEFT
};

enum s_SmoothType
{
	SMOOTH_MEAN,			// 均值滤波
	SMOOTH_MEDIAN,			// 中值滤波
	SMOOTH_GUSSIAN,			// 高斯滤波
	SMOOTH_BILATERAL		// 双边滤波
};

// 视图图像来源
enum VIEW2D_SRC
{
	VIEW_SRC_ORIGIN = 0x01,	// 原始视图
	VIEW_SRC_LEFT,			// 左侧重建
	VIEW_SRC_FRONT			// 前侧重建
};

enum VIEW2D_TYPE
{
	VIEW2D_UNKNOW = 0x00,  // 未知
	VIEW2D_AXIAL,		   // 轴位面（横断面）头->脚
	VIEW2D_CORONAL,		   // 冠状面，(额状面) 前->后
	VIEW2D_SAGITTAL		   // 矢状面 左->右
};

enum VIEW2D_OTHER
{
	AXIAL_OTHER,
	CORONAL_OTHER,
	SAGITTAL_OTHER,
	ALL_VIEW
};

enum VIEW_MODE
{
	VIEW_NATIVE = 0x00,    // 本地模式，只有一个视图查看原始图像
	VIEW_MPR,			   // 有重建的横状位，冠状位和矢状位
	VIEW_3D				   // 横状位，冠状位和矢状位+3D视图
};

enum MODE_VIEW_IDX
{
	VIEW_ORIGIN = 0x00,			// 原始视图
	VIEW_MPR_LEFT,				// 上视图
	VIEW_MPR_FRONT,				// 下视图
	VIEW_3D_IDX,				// 右下3D视图
	VIEW_ALL					// 全部视图
};

enum VIEW_CUESOR
{
	CUESOR_ARROW=0x00,
	CUESOR_CROSS,
	CURSOR_HAND,
	CURSOR_MOVE,
	CURSOR_SCALE,
	CUESOR_LOOK,
	CUESOR_PEN,
};

enum WINDOW_THEME
{
	THEME_DARK,
	THEME_LIGHT
};

enum TOOL_IDX
{
	TOOL_SEL,				// 选择工具
	TOOL_CTLEVEL,			// 窗宽窗位调节
	TOOL_MOVE,				// 移动图像
	TOOL_SCALE,				// 缩放图像
	TOOL_LOOK,				// 放大镜
	TOOL_TEXT,				// 添加注释文本
	TOOL_HU,				// CT值标注
	TOOL_LENGTH,			// 长度测量
	TOOL_ANGLE,				// 角度测量
	TOOL_RECT,				// 矩形区域CT值测量
	TOOL_ELLIPSE			// 椭圆区域CT值测量
};

enum WINDOW_PRESET
{
	WIN_LUNG,				// 肺窗
	WIN_LIVER,				// 肝
	WIN_BRAIN,				// 脑
	WIN_MEDIASTINUM,		// 纵隔窗
	WIN_CHEST,				// 胸窗
	WIN_BONE				// 骨窗
};

enum MEASURE_TYPE
{
	MEASURE_TEXT,			// 添加文本
	MEASURE_HU,				// 添加HU拾取
	MEASURE_LENGTH,			// 添加长度测量
	MEASURE_ANGLE,			// 添加角度测量
	MEASURE_RECT,			// 添加矩形测量
	MEASURE_ELLIPSE			// 添加圆形测量
};

enum FILE_TYPE
{
	FILE_ALL = 0x01,		
	FILE_PNG = 0x02,
	FILE_JPG = 0x04,
	FILE_TGA = 0x08,
	FILE_BMP = 0x10,
	FILE_GIF = 0x20,
	FILE_TIF = 0x40,
	FILE_DCM = 0x80
};

enum OPERATION_MOUSE
{
	OPER_UNKNOW=0,
	OPER_GET_ACTIVE_UI,
	OPER_GET_WIN_RECT,
	OPER_GET_REGION_RECT
};

enum DRAW_TYPE
{
	OPER_MOVE=0,
	DRAW_TEXT,
	DRAW_ARRAW,
	DRAW_PEN,
	DRAW_RECT,
	DRAW_FILL_RECT,
	DRAW_ELLIPSE,
	DRAW_FILL_ELLIPSE,
	DRAW_UNKNOW
};

enum COLOR_THEME
{
	COLOR_NATURE=0,				//自然生态
	COLOR_TECH,					//现代科技
	COLOR_LUXURY,				//豪华优雅
	COLOR_LIVELY,				//青春活力
	COLOR_COMFORTABLE,			//舒适宁静
	COLOR_TRADITION,			//传统经典
	COLOR_SIMPLE,				//简约极致
	COLOR_ROMANTIC,				//浪漫高雅
	COLOR_CULTURE,				//高尚文明
	COLOR_VEHEMENT,				//强烈醒目
	COLOR_SPLIT,				//分裂互补
	COLOR_WARM,					//温暖阳光
	COLOR_COLD,					//高冷自信
	COLOR_LAVENDER,				//熏衣草香
	COLOR_SERENITY,				//宁静天空
	COLOR_OUTDOORS,				//户外瑜珈
	COLOR_PERFUME,				//诱人香水
	COLOR_HEALTHY,				//健康养生
	COLOR_SKY,					//星空蓝光
	COLOR_CITRUS,				//醒目柑橘
	COLOR_REDWINE,				//浓烈红酒
	COLOR_FAIR,					//农夫市集
	COLOR_AESTHETICS,			//缤纷美学
	COLOR_PINK,					//漂亮粉红
	COLOR_CORAL,				//珊瑚绿植
	COLOR_GREENAESTHETICS,		//绿色美学
	COLOR_RADIANTLILY,			//光彩百合
	COLOR_COOL,					//酷炫群体
	COLOR_COSTUMES,				//时尚服饰
	COLOR_TRAVEL				//惬意旅途
};

/**
* @brief 简单概述
* @brief 图像数据结构
*/
typedef struct
{
	ImageFormat    format;					// 图像格式
	int            width;					// 图像宽度
	int            height;					// 图像高度
	short          channel;                 // 图像通道数
	unsigned long  iBufSize;				// 像素数据缓冲区大小
	uint8* pBuffer = NULL;					// 像素数据
}CImage;

#define CODE_USER_INFO	0x68	// 客户端上报用户信息
#define CODE_USER_UPDATE	0x69	// 服务端更新用户信息

#pragma pack(push)
#pragma pack(1)
/**
* @brief 简单概述
* @brief 图像数据结构
*/
typedef struct
{
	char sExportVoicePath[FILE_PATH_MAX_LEN];
}SnapConfig;

typedef struct USERINFO
{
	unsigned char code;
	unsigned int id;
	char userid[100];
	char usercode[20];
	char installtime[30];
	char recenttime[30];
	char endtime[30];
	bool active;
	unsigned int  usecount;
}USERINFO;

typedef struct USERUPDATE
{
	unsigned char code;
	unsigned int id;
	char usercode[20];
	char endtime[30];
	bool active;
}USERUPDATE;


struct WAVHr
{
	// RIFF Chunk
	char chunkID[4];         // 4 bytes: "RIFF"
	uint32_t chunkSize;      // 4 bytes: 文件总大小 (文件大小 - 8)

	char format[4];          // 4 bytes: "WAVE"

	// fmt Subchunk
	char subchunk1ID[4];     // 4 bytes: "fmt "
	uint32_t subchunk1Size;  // 4 bytes: fmt 子块大小
	uint16_t audioFormat;    // 2 bytes: 音频编码格式，1 表示 PCM
	uint16_t numChannels;    // 2 bytes: 声道数
	uint32_t sampleRate;     // 4 bytes: 采样率
	uint32_t byteRate;       // 4 bytes: 数据速率 (采样率 * 声道数 * 位深度 / 8)
	uint16_t blockAlign;     // 2 bytes: 数据块对齐 (声道数 * 位深度 / 8)
	uint16_t bitsPerSample;  // 2 bytes: 位深度

	// data Subchunk
	char subchunk2ID[4];     // 4 bytes: "data"
	uint32_t subchunk2Size;  // 4 bytes: 音频数据的大小
};
#pragma pack()

#pragma pack(push, 1) //以字节对齐方式进行结构体定义

// 定义RIFF 头结构体
typedef struct RiffChunk
{
	char		RIFF[4];		// 以 "RIFF" 标识，表示整个文件是一个RIFF块
	uint32_t	length;			// 该块的总大小（不包括ID和Size字段自身）
	char		avi_type[4];	// 通常为"AVI "四个字符.
}RiffChunk;

typedef struct ListChunk
{
	//char chunkid[4];			// Chunk ID("LIST")
	unsigned int size;			// 块大小
	char chunktype[4];			// 块类型  FourCC
}ListChunk;

typedef struct ChunkHr
{
	char chunkid[4];				// 块名称字符串
	unsigned int size;				// 本数据结构的大小，不包括最初的8个字节（chunkid和size两个域）
}ChunkHr;

// 定义Main Header Chunk结构体
typedef struct MainHeaderChunk
{
	//char chunkid[4];						// 'avih'
	//unsigned int size;					// 本数据结构的大小，不包括最初的8个字节（chunkid和size两个域）
	unsigned int microSecPerFrame;			// 表示每帧所占用的微秒数
	unsigned int maxBytesPerSec;			// 表示整个文件中每秒的最大数据传输率
	unsigned int paddingGranularity;		// 指定对齐数据所使用的填充大小
	unsigned int flags;						// 表示文件的一些全局标记信息，比如是否含有索引块等
	unsigned int totalFrames;				// 表示文件中的总帧数
	unsigned int initialFrames;				// 表示文件中初始帧数，为交互格式指定初始帧数（非交互格式应该指定为0）
	unsigned int streams;					// 表示文件中的流个数
	unsigned int suggestedBufferSize;		// 建议缓冲区的大小(应能容纳最大的块)
	unsigned int width;						// 表示图像或视频的宽度,像素为单位
	unsigned int height;					// 表示图像或视频的高度,像素为单位
	unsigned int reserved[4];				// 保留字段
}MainHeaderChunk;

// 定义Stream Header Chunk结构体
typedef struct StreamHeaderChunk
{
	char streamType[4];				// 标识流的类型，如"auds"表示音频流，"vids"表示视频流
	char handler[4];				// 标识处理该流的处理器类型，例如，音频流可能使用"PCM "表示无压缩的音频数据
	unsigned int flags;				// 表示流的一些标志信息
	unsigned int priority;			// 指定流的优先级
	unsigned int language;			// 表示流的语言
	unsigned int initialFrames;		// 指定初始帧数
	unsigned int scale;				// 流的时间刻度，即每个时间单位所占的帧数
	unsigned int rate;				// 流的采样率
	unsigned int start;				// 开始时间
	unsigned int length;			// 流的长度(以帧为单位)
	unsigned int quality;			// 流的质量(0-10000)
	unsigned int sampleSize;		// 每个采样（帧）的大小
	unsigned short left;			// 视频主窗口中的显示位置
	unsigned short top;				// 视频主窗口中的显示位置
	unsigned short right;			// 视频主窗口中的显示位置
	unsigned short bottom;			// 视频主窗口中的显示位置
}StreamHeaderChunk;

// "strf"子块紧跟在"strh"子块之后，其结构视"strh"子块的类型而定，如果 strh子块是视频数据流，则 strf子块的内容是一个与windows设备无关位图的BIMAPINFO结构
// strf 块
typedef struct VideoChunk
{
	char chunkid[4];				// 必须为‘strf’
	unsigned int size;				// 本数据结构的大小，不包括最初的8个字节（chunkid和size两个域）
	unsigned int chunksize;
	int biWidth;					// 视频图像宽度
	int biHeight;					// 视频图像高度
	short biPlanes;
	short biBitCount;
	int biCompression;
	int biSizeImage;				// 每帧图像大小
	int biXPelsPerMeter;			// 每帧图像水平像素值
	int biYPelsPerMeter;			// 每帧图像垂直像素值
	int biClrUsed;
	int biClrImportant;
}VideoChunk;

// 38字节
typedef struct VoiceChunk	// 26字节
{
	char	 chunkid[4];				// 必须为‘strf’
	unsigned int biSize;
	unsigned short wFormatTag;
	unsigned short nChannels;			// 声道数
	unsigned int nSamplesPerSec;		// 采样率
	unsigned int nAvgBytesPerSec;		// 每秒的数据量
	unsigned short nBlockAlign;			// 数据块对齐标志
	unsigned short wBitsPerSample;		// 每次采样的数据量
	unsigned short cbSize;				// 大小
}VoiceChunk;

// 填充文件中的未使用空间
typedef struct JunkChunk
{
	char			chunkid[4];			// 必须为‘junk’
	unsigned int	size;				// 本数据结构的大小，不包括最初的8个字节（chunkid和size两个域）
}JunkChunk;

typedef struct VprpChunk
{
	unsigned int videoFormat;				// 视频流的格式 FourCC
	unsigned int videoStandard;				// 视频流的标准
	unsigned int dwVerticalRefreshRateN;	// 帧率分子
	unsigned int dwVerticalRefreshRateD;	// 帧率分母
	unsigned int dwHTotalInT;				// 每个扫描线的总像素数
	unsigned int dwVTotalInLines;			// 显示的总扫描线数
	unsigned int dwFrameAspectRatioX;		// 帧宽度的比例因子
	unsigned int dwFrameAspectRatioY;		// 帧高度的比例因子
	unsigned int dwFrameWidthInPixels;		// 帧宽度（像素）
	unsigned int dwFrameHeightInLines;		// 帧高度（行）
	unsigned int dwCompressBufferHints;		// 压缩缓冲区建议大小
	unsigned int dwBufferSize;				// 缓冲区大小
	unsigned int dwMinFrameSize;			// 帧的最小大小
	unsigned int dwMaxFrameSize;			// 帧的最大大小
}VprpChunk;


// 视频预览帧
typedef struct VideoPropHeader
{
	char propFourCC[4];
	uint32_t cbSize;
	uint32_t width;
	uint32_t height;
	uint32_t planes;
	uint32_t bitCount;
}VideoPropHeader;

/* MP4文件格式的数据结构定义 */
typedef struct FileHr
{
	char	ftype[4];
}FileHr;

#pragma pack(pop)