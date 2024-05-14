#define _CRT_SECURE_NO_WARNINGS

#include "logtxt.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <io.h>
#include <direct.h>
#include <stdarg.h>

std::string GeneralCurTime(std::string format)
{
	time_t timep; time(&timep); char tmp[20];
	strftime(tmp, sizeof(tmp), format.c_str(), localtime(&timep));
	return tmp;
}

LogTxt* LogTxt::GetMgr()
{
	static LogTxt s;
	return &s;
}

LogTxt::~LogTxt()
{
}

LogTxt::LogTxt()
{
	m_run = true;
	m_lasttime = 0;
	m_idate = 10;
	m_LogType[LV_TRACE] = "[TRACE]";
	m_LogType[LV_DEBUG] = "[DEBUG]";
	m_LogType[LV_INFO]  = "[INFO]";
	m_LogType[LV_WARN]  = "[WARN]";
	m_LogType[LV_ERROR] = "[ERROR]";
	m_LogType[LV_FATAL] = "[FATAL]";
}

/**
* @brief 简单概述
* @brief 设置日志输出目录
*/
void LogTxt::SetFolder(std::string path)
{
	if(path.empty())
		path = LOGFOLDER;
	path.append("/");
	m_FolderName = path;
	CreateFolder(m_FolderName);
	IsFileExis(m_FolderName);

	// 多线程清理过期日志
	std::thread clear_thread(&LogTxt::ClearOldLogFile, this);
	clear_thread.detach();
}

std::vector<std::string> SplitStr(std::string str, std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	//扩展字符串以方便操作
	str += pattern;
	int size = str.size();
	for (int i = 0; i < size; i++)
	{
		pos = str.find(pattern, i);
		if (pos < size)
		{
			std::string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}

/**
* @brief 简单概述
* @brief 检查文件夹是否存在，不存在则创建
*/
void LogTxt::CreateFolder(std::string path)
{
	if (_access(path.c_str(), 0) == 0)
		return;

	std::vector<std::string> _vec = SplitStr(path, "/");
	std::string temp_path;
	for (auto itr = _vec.begin(); itr != _vec.end(); itr++)
	{
		temp_path.append(*itr).append("/");
		if (_access(temp_path.c_str(), 0) != 0)
		{
			_mkdir(temp_path.c_str());
		}
	}
}

/**
* @brief 简单概述
* @brief 日志文件是否存在，不存在则创建
*/
bool LogTxt::IsFileExis(std::string path)
{
	m_FileName = GetTodayFileName(path);
	if (_access(m_FileName.c_str(), 0) != 0)
	{
		std::fstream out;
		out.open(m_FileName.c_str(), std::ios_base::app);
		out.close();
		return false;
	}
	return true;
}

/**
* @brief 简单概述
* @brief 获取当前日志文件名
*/
std::string LogTxt::GetTodayFileName(std::string folder)
{
	folder.append(LOGFILE_FREX).append(GeneralCurTime("%Y-%m-%d")).append(".LOG");
	return folder;
}

/**
* @brief 简单概述
* @brief 增加输出内容
*/
void LogTxt::WriteLog(const char *p_lpFile, const  unsigned  long  lLine, const short type, const char * msg,...)
{
	m_Mutex.lock();
	IsFileExis(m_FolderName);
	char _time[30];
	snprintf(_time, sizeof(_time), " [%s] ", GeneralCurTime("%Y-%m-%d %H:%M:%S").c_str());
	std::string message = _time;
	std::string file = p_lpFile;
	int idx = file.find_last_of("\\") + 1;
	message.append(file.substr(idx, file.size()-idx));
	char line[10];
	snprintf(line,sizeof(line)," (%d) ", lLine);
	message.append(line);
	message.append(m_LogType[type]);
	message.append(":");

	va_list   vlist; 
	va_start(vlist, msg);
	char buffer[1024 * 20] = {'\0'};
	vsnprintf(buffer, sizeof(buffer), msg, vlist);
	va_end(vlist);

	message.append(buffer);
	std::cout << message << std::endl;
	this->WriteMsg(message);
	m_Mutex.unlock();
}

bool LogTxt::WriteMsg(std::string msg)
{
	std::ofstream out;
	out.open(m_FileName, std::ios::app);
	if (out)
	{
		out << msg << std::endl;
		out.close();
		return true;
	}
	else
	{
		out.close();
		return false;
	}
}

/**
* @brief 简单概述
* @brief 获取以前的日期文件
*/
std::string LogTxt::GetClearFile(int m)
{
	char _time[12];
	time_t tt;time(&tt);
	tt -= m * 86400/*24 * 60 * 60*/;
	struct tm *_tm;
	_tm = localtime(&tt);
	strftime(_time, 64, "%Y-%m-%d", _tm);
	std::string s = LOGFILE_FREX + std::string(_time) + ".LOG";
	return s;
}

/**
* @brief 简单概述
* @brief 清理老的日志文件
*/
void LogTxt::ClearOldLogFile()
{
	_finddata_t file_info;
	std::string current_path = m_FolderName + "*.LOG";
	long long handle = _findfirst(current_path.c_str(), &file_info);
	if (-1 != handle)
	{
		std::string datetime = GetClearFile(m_idate);
		std::vector<std::string> vec_path;
		do
		{
			if (file_info.attrib != _A_SUBDIR)
			{
				std::string filename = file_info.name;
				if (filename < datetime)
				{
					// 记录删除的文件路径
					std::string abspath = m_FolderName + filename;
					vec_path.push_back(abspath);
				}
			}
		} while (!_findnext(handle, &file_info));								  
		_findclose(handle);
		for (auto itr = vec_path.begin(); itr != vec_path.end(); itr++)
		{
			std::string path = *itr;
			if (remove(path.c_str()) != 0)
			{
				LOGOUT_ERROR("delete log file fail.file path:%s.", itr->c_str());
			}
		}
	}
}