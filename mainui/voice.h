#pragma once
#include <string>
#include <QString>
#include <QObject>
#include <QTextToSpeech>

class Voice : public QObject
{
	Q_OBJECT
public:
	Voice();
	~Voice();

	void SpeakWinAPI(QString text);
	void SpeakQtAPI(QString text);
	void ConvertWAVFile(QString path, QString text);
	void ConvertWAVFileWinApi(QString path, QString text);

	std::map<int,QString> GetVoiceAvailable();

private slots:
	void SpeakQtFinish(QTextToSpeech::State);

private:
	QTextToSpeech* m_pTextSpeech;
};