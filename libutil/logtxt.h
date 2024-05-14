#pragma once
#include <string>
#include <thread>
#include <queue>
#include <mutex>
#include <vector>
#include "util.h"
#include "public.h"

#define LOGFOLDER "LOG_FILE"
#define LOGFILE_FREX "Log_"
#define LOGINS LogTxt::GetMgr()

typedef enum
{
	LV_TRACE = 0x0000,	// 跟踪类型
	LV_DEBUG = 0x0001,	// 调试类型
	LV_INFO = 0x0002,	// 信息类型
	LV_WARN = 0x0003,	// 警告类型
	LV_ERROR = 0x0004,	// 错误类型
	LV_FATAL = 0x0005	// 致命类型
}LEVEL_E;

#define LOGOUT_TRACE(format,...) LOGINS->WriteLog(__FILE__,__LINE__,LV_TRACE,format,## __VA_ARGS__)
#define LOGOUT_DEBUG(format,...) LOGINS->WriteLog(__FILE__,__LINE__,LV_DEBUG,format,## __VA_ARGS__)
#define LOGOUT_INFO(format,...)  LOGINS->WriteLog(__FILE__,__LINE__,LV_INFO, format,## __VA_ARGS__)
#define LOGOUT_WARN(format,...)  LOGINS->WriteLog(__FILE__,__LINE__,LV_WARN, format,## __VA_ARGS__)
#define LOGOUT_ERROR(format,...) LOGINS->WriteLog(__FILE__,__LINE__,LV_ERROR,format,## __VA_ARGS__)
#define LOGOUT_FATAL(format,...) LOGINS->WriteLog(__FILE__,__LINE__,LV_FATAL,format,## __VA_ARGS__)

class COMMON_EXPORT LogTxt
{
public:
	static LogTxt* GetMgr();

	/**
	* @brief 简单概述
	* @brief 设置日志输出目录
	*/
	void SetFolder(std::string path);

	/**
	* @brief 简单概述
	* @brief 设置日志保留的天数
	*/
	void SetDateNum(int m) { m_idate = m; }

	/**
	* @brief 简单概述
	* @brief 检查文件夹是否存在，不存在则创建
	*/
	void CreateFolder(std::string path);

	/**
	* @brief 简单概述
	* @brief 日志文件是否存在，不存在则创建
	*/
	bool IsFileExis(std::string path);

	/**
	* @brief 简单概述
	* @brief 清理老的日志文件
	*/
	void ClearOldLogFile();

	/**
	* @brief 简单概述
	* @brief 增加输出内容
	*/
	void WriteLog(const char *p_lpFile, const  unsigned  long  lLine,const short type, const char* msg,...);

private:
	LogTxt();
	~LogTxt();

	bool WriteMsg(std::string msg);

	/**
	* @brief 简单概述
	* @brief 获取当前日志文件名
	*/
	std::string GetTodayFileName(std::string folder);

	/**
	* @brief 简单概述
	* @brief 获取以前的日期文件
	*/
	std::string GetClearFile(int);

	bool			m_run;
	int				m_idate;
	unsigned int	m_lasttime;

	std::mutex		m_Mutex;
	std::string		m_FileName;
	std::string		m_FolderName;
	std::string		m_LogType[6];
};