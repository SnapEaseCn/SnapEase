#pragma once
#include "public.h"
#include <map>
#include <fstream>

#define GLOB_INS Glob::GetIns()
class COMMON_EXPORT Glob
{
public:
	static Glob* GetIns()
	{
		static Glob ins;
		return &ins;
	}

	std::string GetFormatName(int id)
	{
		std::string format;
		if (map_format.find(id) != map_format.end())
		{
			format = map_format[id];
		}
		return format;
	}

	void SetAppPath(std::string path)
	{
		m_AppPath = path;
	}

	void InitData()
	{
		std::string FileName = m_AppPath + "/config.dll";
		std::ifstream file(FileName);
		if (!file.is_open())	// 文件不存在
		{
			std::ofstream file(FileName, std::ios::out | std::ios::app);
		}
		else
		{
			// 打开二进制文件
			std::ifstream file(FileName, std::ios::binary);
			// 获取文件大小
			file.seekg(0, std::ios::end);
			std::size_t file_size = file.tellg();
			file.seekg(0, std::ios::beg);
			// 创建缓冲区，用于存储文件内容
			char* buffer = new char[file_size];
			// 读取文件内容到缓冲区
			file.read(buffer, file_size);
			memset(&m_SnapConfig, 0, sizeof(m_SnapConfig));
			memcpy(&m_SnapConfig, buffer,sizeof(m_SnapConfig));
			delete[] buffer;
		}
	}

	bool SetExportVoicePath(std::string path)
	{
		if (path.size() <= sizeof(m_SnapConfig.sExportVoicePath))
		{
			memset(m_SnapConfig.sExportVoicePath, 0, sizeof(m_SnapConfig.sExportVoicePath));
			memcpy(m_SnapConfig.sExportVoicePath, path.c_str(), path.size());
			return true;
		}
		else
			return false;
	}

	std::string GetExportVoicePath()
	{
		return m_SnapConfig.sExportVoicePath;
	}

	bool SaveConfigData()
	{
		std::string FileName = m_AppPath + "/config.dll";
		std::ifstream file(FileName);
		if (!file.is_open())	// 文件不存在
		{
			std::ofstream file(FileName, std::ios::out | std::ios::app);
		}
		std::ofstream outfile(FileName, std::ios::binary | std::ios::out);
		outfile.write((const char*)(&m_SnapConfig), sizeof(m_SnapConfig));

		return true;
	}

private:
	Glob()
	{
		map_format[FILE_PNG] = "png";
		map_format[FILE_JPG] = "jpg";
		map_format[FILE_TGA] = "tga";
		map_format[FILE_BMP] = "bmp";
		map_format[FILE_GIF] = "gif";
		map_format[FILE_TIF] = "tif";
		map_format[FILE_DCM] = "dcm";
	}
	~Glob()
	{

	}

private:
	/**
	* @brief 简单概述
	* @brief 支持的格式
	*/
	std::map<int, std::string>	map_format;

	/**
	* @brief 简单概述
	* @brief 当前可执行文件路径
	*/
	std::string  m_AppPath;

	/**
	* @brief 简单概述
	* @brief 配置文件对象
	*/
	SnapConfig	m_SnapConfig;
};