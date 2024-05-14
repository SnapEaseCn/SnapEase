#include "voice.h"
#include "text.h"
#include <public.h>
#include <sapi.h>
#include <sphelper.h>
#include <locale>
#include <fstream>
#include <iostream>
#include <Windows.h>
#include <QTextToSpeech>
#include <QAudioEncoderSettings>
#include <QAudioRecorder>
#include <QBuffer>
#include <QFile>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QUrl>

Voice::Voice()
{
	m_pTextSpeech = new QTextToSpeech(this);
	connect(m_pTextSpeech, SIGNAL(stateChanged(QTextToSpeech::State)), this, SLOT(SpeakQtFinish(QTextToSpeech::State)));
}

Voice::~Voice()
{

}

void Voice::SpeakWinAPI(QString text)
{
	HRESULT hr = CoInitialize(nullptr);
	if (SUCCEEDED(hr))
	{
		ISpVoice* pVoice;
		hr = CoCreateInstance(CLSID_SpVoice, nullptr, CLSCTX_ALL, IID_ISpVoice, (void**)&pVoice);
		if (SUCCEEDED(hr)) 
		{
			// 设置语音属性，如音量和语速
			pVoice->SetVolume(100);  // 设置音量（0-100）
			pVoice->SetRate(0);     // 设置语速（-10至10）

			// 文本转换为语音
			pVoice->Speak(QStr2LPCWSTR(text), 0, nullptr);
			pVoice->Release();
		}
		CoUninitialize();
	}
}

void Voice::SpeakQtAPI(QString text)
{
	// 获取所有可用的语音引擎
	QVector<QVoice> availableVoices = m_pTextSpeech->availableVoices();
	QStringList availableEngine = QTextToSpeech::availableEngines();
	if (availableVoices.size() > 0)
	{
		// 设置语音引擎为默认引擎
		m_pTextSpeech->setVoice(availableVoices[0]);
	}

	// 设置音量和语速
	m_pTextSpeech->setVolume(100.0);
	m_pTextSpeech->setRate(0.0);

	// 将文本转为语音并播放
	m_pTextSpeech->say(text);
}

std::map<int, QString> Voice::GetVoiceAvailable()
{
	// 获取所有可用的语音引擎
	QVector<QVoice> availableVoices = m_pTextSpeech->availableVoices();
	int idx = 0;
	std::map<int, QString> availables;
	for(const auto& itr: availableVoices)
	{
		int gender = itr.gender();
		QString gender_name = tr("Male");
		if (gender == QVoice::Male)
		{
			gender_name = tr("Male");
		}
		else if (gender == QVoice::Female)
		{
			gender_name = tr("Female");
		}
		else
		{
			gender_name = tr("Unknown");
		}
		int age = itr.age();
		QString age_name = tr("Child");
		if (age == QVoice::Child)
		{
			age_name = tr("Child");
		}
		else if (age == QVoice::Teenager)
		{
			age_name = tr("Teenager");
		}
		else if (age == QVoice::Adult)
		{
			age_name = tr("Adult");
		}
		else if (age == QVoice::Senior)
		{
			age_name = tr("Senior");
		}
		else
		{
			age_name = tr("Other");
		}
		availables[idx] = gender_name + Str2QStr(" ") + age_name;
		idx++;
	}
	return availables;
}

void Voice::SpeakQtFinish(QTextToSpeech::State state)
{
	if (state == QTextToSpeech::Ready)
	{
		// 文本转语音已准备就绪
	}
	else if (state == QTextToSpeech::Speaking) 
	{
		// 正在播放语音
	}
	else if (state == QTextToSpeech::Paused)
	{
		// 播放语音已暂停
	}
	else if (state == QTextToSpeech::BackendError)
	{
		// 播放语音被中断
	}
}

void Voice::ConvertWAVFile(QString path, QString text)
{
	// 获取所有可用的语音引擎
	QVector<QVoice> availableVoices = m_pTextSpeech->availableVoices();
	QStringList availableEngine = QTextToSpeech::availableEngines();

	if (availableVoices.size() > 0)
	{
		// 设置语音引擎为默认引擎
		m_pTextSpeech->setVoice(availableVoices[0]);
	}

	// 设置音量和语速
	m_pTextSpeech->setVolume(100.0);
	m_pTextSpeech->setRate(0.0);

	// 将文本转为语音并播放
	m_pTextSpeech->say(text);

	WAVHr	hr;
	strncpy(hr.chunkID, "RIFF", sizeof(hr.chunkID));
	hr.chunkSize = sizeof(WAVHr);		// 头结构大小+音频数据大小
	strncpy(hr.format, "WAVE", sizeof(hr.format));
	strncpy(hr.subchunk1ID, "fmt", sizeof(hr.subchunk1ID)-1);
	hr.subchunk1Size = 28;
	hr.audioFormat = 1;
	hr.numChannels = 2;
	hr.sampleRate = 1;		// 采样率

	strncpy(hr.subchunk2ID, "data", sizeof(hr.subchunk2ID));
	hr.subchunk2Size = 1;

	std::string file_path = QStr2Str(path);
	std::ofstream outfile(file_path, std::ios::binary);

	if (!outfile) 
	{
		// 文件打开失败，创建文件
		std::ofstream file(file_path, std::ios::out | std::ios::app);
	}

	// 要写入的数据
	double doubleValue = 3.14159;

	// 将数据以二进制形式写入文件
	outfile.write(reinterpret_cast<const char*>(&hr), sizeof(hr));
	outfile.write(reinterpret_cast<const char*>(&doubleValue), sizeof(double));

	// 关闭文件流
	outfile.close();
}

void Voice::ConvertWAVFileWinApi(QString path, QString text)
{
	// 初始化 COM 组件
	CoInitialize(nullptr);

	// 创建 SAPI 对象
	ISpVoice *pVoice = nullptr;
	HRESULT hr = CoCreateInstance(CLSID_SpVoice, nullptr, CLSCTX_ALL, IID_ISpVoice, reinterpret_cast<void**>(&pVoice));
	if (FAILED(hr))
	{
		std::cout << "Failed to create SAPI voice" << std::endl;
		CoUninitialize();
		return;
	}

	//生成WAV文件
	CComPtr<ISpStream> cpISpStream;
	CComPtr<ISpStreamFormat> cpISpStreamFormat;
	CSpStreamFormat spStreamFormat;
	pVoice->GetOutputStream(&cpISpStreamFormat);
	spStreamFormat.AssignFormat(cpISpStreamFormat);
	HRESULT hResult = SPBindToFile(QStr2LPCWSTR(path),SPFM_CREATE_ALWAYS,&cpISpStream,&spStreamFormat.FormatId(),
		spStreamFormat.WaveFormatExPtr());
	if (SUCCEEDED(hResult))
	{
		pVoice->SetOutput(cpISpStream, TRUE);
		hr = pVoice->Speak(QStr2LPCWSTR(text), SPF_DEFAULT, NULL);
		if (FAILED(hr))
		{
			std::cout << "Failed to convert text to audio" << std::endl;
		}
		else
		{
			std::cout << "Audio saved to output.wav" << std::endl;
		}
	}
	else if (FAILED(hr))
	{
		std::cout << "Failed to create audio stream" << std::endl;
		pVoice->Release();
		CoUninitialize();
		return;
	}

	// 关闭音频流
	cpISpStream->Close();

	// 释放资源
	pVoice->Release();
	CoUninitialize();
}