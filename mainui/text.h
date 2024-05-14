#pragma once
#include <windows.h>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <QString>
#include <algorithm>
#include "public.h"
#include <ctime>
#include <sstream>
#include <codecvt>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cctype>

#include <sys/stat.h>
#include <sys/types.h>
#include <direct.h>

#pragma comment(lib, "iphlpapi.lib")

#define SNAPEASE_FOLDER		"C:/ProgramData/GLOB/"
#define SNAPEASE_FILE		"glob.dll"

inline std::string RemoveFilePart(const std::string& filePath)
{
	std::string folder;
	// 找到最后一个路径分隔符的位置
	std::size_t found = filePath.find_last_of("/\\");
	if(found != std::string::npos) 
	{
		// 去掉文件部分，并保留路径分隔符
		folder = filePath.substr(0, found + 1);
	}
	return folder;
}

inline std::string GenerateTimeStr()
{
	// 获取当前时间
	std::time_t now = std::time(nullptr);

	// 转换为本地时间
	std::tm* localTime = std::localtime(&now);

	// 创建一个字符串流对象
	std::ostringstream oss;

	// 将年、月、日、时、分、秒转换为字符串，并将其追加到字符串流对象中
	oss << localTime->tm_year + 1900 << localTime->tm_mon + 1 << localTime->tm_mday
		<< localTime->tm_hour << localTime->tm_min << localTime->tm_sec;

	// 返回生成的唯一字符串
	return oss.str();
}

inline QString Str2QStr(const std::string& str)
{
	return QString::fromLocal8Bit(str.data());
}

inline std::string QStr2Str(const QString& qstr)
{
	QByteArray cstr = qstr.toLocal8Bit();
	return std::string(cstr);
}

inline std::wstring QStr2WStr(QString text)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::wstring wstr = converter.from_bytes(text.toUtf8().constData());
	return wstr;
}

inline std::wstring Str2WStr(const std::string& str)
{
	int length = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	std::vector<wchar_t> buffer(length);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buffer.data(), length);
	return std::wstring(buffer.data());
}

inline LPCWSTR	QStr2LPCWSTR(QString text)
{
	return reinterpret_cast<LPCWSTR>(text.utf16());
}

// 获取设备的物理网卡地址
inline std::vector<std::string> GetMacAddr()
{
	// 获取设备的物理网卡地址
	std::vector<std::string> macAddresses;

	ULONG bufferSize = 0;
	if (GetAdaptersInfo(nullptr, &bufferSize) != ERROR_BUFFER_OVERFLOW) 
	{
		return macAddresses;
	}

	std::vector<unsigned char> buffer(bufferSize);
	PIP_ADAPTER_INFO adapterInfo = reinterpret_cast<PIP_ADAPTER_INFO>(buffer.data());
	if (GetAdaptersInfo(adapterInfo, &bufferSize) != ERROR_SUCCESS) 
	{
		return macAddresses;
	}

	while (adapterInfo != nullptr) 
	{
		std::string macAddress;
		for (DWORD i = 0; i < adapterInfo->AddressLength; i++) 
		{
			macAddress += (i > 0 ? ":" : "") + std::to_string(adapterInfo->Address[i]);
		}
		macAddresses.push_back(macAddress);
		adapterInfo = adapterInfo->Next;
	}

	return macAddresses;
}

// 获取硬盘序列号
inline std::string GetHardDiskSerialNum()
{
	std::string serialNumber;

#ifdef _WIN32
	DWORD volumeSerialNumber;
	GetVolumeInformation(L"C:\\", nullptr, 0, &volumeSerialNumber, nullptr, nullptr, nullptr, 0);

	std::stringstream ss;
	ss << std::hex << volumeSerialNumber;
	serialNumber = ss.str();
#endif

	return serialNumber;
}

// 生成唯一标识字符串
inline std::string GenerateUniqueIdentifier()
{
	std::ostringstream oss;

	// 获取网卡的物理地址
	std::vector<std::string> macAddrs = GetMacAddr();

	if (macAddrs.size() > 0)
	{
		oss << macAddrs[0]; // 网卡的物理地址
	}

	std::string disk_serial = GetHardDiskSerialNum();

	if (!disk_serial.empty())
	{
		oss << disk_serial;
	}

	// 返回生成的唯一标识字符串（取前100个字符）
	std::string identifier = oss.str();
	return identifier.substr(0, 100);
}