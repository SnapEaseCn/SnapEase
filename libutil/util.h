/*****************************************************************************
*  C++ 数据定义
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
#pragma once
#include <string>
#include <vector>
#include <corecrt_math_defines.h>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cctype>
#include <cstring>
#include <Windows.h>
#include <time.h>

/**
* @brief 简单概述
* @brief 关于我们
* @brief str:需要处理的字符串 将target替换成src
*/
inline void ReplaceStr(std::string* str, const std::string& target, const std::string& src)
{
	std::size_t start = 0;
	while ((start = str->find(target, start)) != std::string::npos)
	{
		str->replace(start, target.length(), src);
		start += src.length();
	}
}

inline std::string Hex2Str(const char* data, size_t dataSize)
{
	std::stringstream ss;
	// 设置流输出为大写模式
	ss << std::uppercase;
	for (size_t i = 0; i < dataSize; ++i)
	{
		// 将每个字符按照两位16进制输出
		ss << std::hex << std::setw(2) << std::setfill('0');
		// 将字母转换为大写并输出
		ss << static_cast<int>(std::toupper(data[i]));
	}

	return ss.str();
}

// 获取系统当前时间
inline std::string GeneralCurTime()
{
	time_t timep; time(&timep); char tmp[20];
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
	return tmp;
}

// 字符串分割
inline std::vector<std::string> Split(const std::string& str, char delim)
{
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;
	while (std::getline(ss, token, delim))
	{
		tokens.push_back(token);
	}
	return tokens;
}

inline std::string ReadFile(const std::string& fileName)
{
	std::ifstream file(fileName);
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return content;
}

inline void WriteFile(const std::string& fileName, const std::string& content)
{
	std::ofstream file(fileName);
	file << content;
}

inline std::string Trim(const std::string& str)
{
	size_t first = str.find_first_not_of(' ');
	if (std::string::npos == first)
	{
		return str;
	}
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}

inline std::string FormatTime(const std::time_t& time, const std::string& format)
{
	std::stringstream ss;
	ss << std::put_time(std::localtime(&time), format.c_str());
	return ss.str();
}