#include "RecordDlg.h"
#include <QtConcurrent>
#include <QScreen>
#include <QApplication>
#include <QDesktopWidget>
#include <QPixmap>
#include <fstream>
#include "ScreenDlg.h"
#include "text.h"
#include "public.h"

RecordDlg::RecordDlg(QWidget *parent):QDialog(parent)
{
	ui.setupUi(this);
	Qt::WindowFlags flags = windowFlags();
	flags |= Qt::WindowMinMaxButtonsHint;
	flags |= Qt::WindowCloseButtonHint;
	this->setWindowFlags(flags);

	m_timer = new QTimer();

	// ��ȡ����·��
	QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)+"/";
	QString file = desktopPath+Str2QStr(GenerateTimeStr() + ".avi");
	ui.lineEdit->setText(file);

	connect(ui.pbStart, SIGNAL(clicked()), this, SLOT(StartRecord()));
	connect(ui.pbClose, SIGNAL(clicked()), this, SLOT(close()));

	// just for test.
	ReadAviDemo();
}

RecordDlg::~RecordDlg()
{
}

void RecordDlg::StartRecord()
{
	if (ui.rbRect->isChecked())
	{
		this->setWindowOpacity(0.0); // ���ò�͸����(��Χ��0 ��1)
		// ����һ����Ļ����
		QScreen *screen = QGuiApplication::primaryScreen();
		// ��ͼ
		QPixmap screenshot = screen->grabWindow(0);
		this->setWindowOpacity(0.0); // ���ò�͸����(��Χ��0 ��1)
		ScreenDlg* dlg = new ScreenDlg(this);
		connect(dlg, SIGNAL(FinishSelRect(QRect)), this, SLOT(FinishDrawRect(QRect)));
		dlg->SetPixMap(screenshot);
		dlg->show();
	}
	else if (ui.rbDeskTop->isChecked())
	{

	}
	else if (ui.rbFullScreen->isChecked())
	{

	}
}

void RecordDlg::FinishDrawRect(QRect rect)
{
	connect(m_timer, &QTimer::timeout, [&]() 
	{
	// ��ȡָ���������Ļ��ͼ
	QScreen *screen = QGuiApplication::primaryScreen();
	QPixmap screenshot = screen->grabWindow(QApplication::desktop()->winId(),
			rect.x(), rect.y(), rect.width(), rect.height());
	QImage image = screenshot.toImage();
	}
	);
	m_timer->start(100);
}

void RecordDlg::ReadAviDemo()
{
	QString path = QString::fromLocal8Bit("D:/ý���ļ�/1087.avi");

	// �򿪶������ļ�
	std::ifstream file(QStr2Str(path), std::ios::binary);
	if (!file.is_open())
		return;
	
	// ��ȡ�ļ���С
	file.seekg(0, std::ios::end);
	std::size_t file_size = file.tellg();
	file.seekg(0, std::ios::beg);

	// ���������������ڴ洢�ļ�����
	RiffChunk avi_hr;
	memset(&avi_hr,0,sizeof(RiffChunk));
	char* buffer = new char[sizeof(RiffChunk)];
	file.read(buffer, sizeof(RiffChunk));
	memcpy(&avi_hr, buffer,sizeof(RiffChunk));
	delete[] buffer;

	while (!file.eof())
	{
		char mark[4];
		file.read(mark, sizeof(mark));
		if (memcmp(mark, "LIST", 4) == 0)
		{
			unsigned int length = 0;
			file.read((char*)&length, sizeof(length));
			char info[4];
			file.read(info, 4);
			if (memcmp(info, "movi", 4) == 0)
			{
				//ParseMoviData(file, length);
				int mm = 0;
			}
		}
		else
		{
			int length=0;
			file.read((char*)&length, sizeof(length));
			ReadAviChunk(file, mark, length);
		}
	}
}

void RecordDlg::ReadAviChunk(std::ifstream& file,char* chunkid, int size)
{
	if (memcmp(chunkid, "avih", 4) == 0)
	{
		MainHeaderChunk main_chunk;
		memset(&main_chunk, 0, sizeof(main_chunk));
		file.read((char*)&main_chunk, sizeof(main_chunk));
	}
	else if (memcmp(chunkid, "strh", 4) == 0)
	{
		StreamHeaderChunk chunk;	// 56�ֽ�
		memset(&chunk, 0, sizeof(chunk));
		file.read((char*)&chunk, sizeof(chunk));
		int mm = 0;

		if (memcmp(chunk.streamType, "vids", 4) == 0)
		{
			VideoChunk vChunk;
			memset(&vChunk, 0, sizeof(vChunk));
			file.read((char*)&vChunk, sizeof(vChunk));
			int mm = 0;
		}
		else if (memcmp(chunk.streamType, "auds", 4) == 0)
		{
			/*VoiceChunk cChunk;
			memset(&cChunk, 0, sizeof(cChunk));
			file.read((char*)&cChunk, sizeof(cChunk));*/
			file.seekg(38, std::ios::cur);
			int mm = 0;
		}
	}
	else if (memcmp(chunkid, "JUNK", 4) == 0)
	{
		file.seekg(size, std::ios::cur);
	}
	else if (memcmp(chunkid, "vprp", 4) == 0)
	{
		/*VprpChunk chunk;
		memset(&chunk, 0, sizeof(chunk));
		file.read((char*)&chunk, sizeof(chunk));*/

		file.seekg(size, std::ios::cur);
		int mm = 0;
	}
	else if (memcmp(chunkid, "ISFT", 4) == 0)
	{
		/*VprpChunk chunk;
		memset(&chunk, 0, sizeof(chunk));
		file.read((char*)&chunk, sizeof(chunk));*/

		file.seekg(size, std::ios::cur);
		int mm = 0;
	}
	else
	{
		ParseMoviData(file, chunkid, size);
	}
}

// ��������Ƶ����  ��Ƶ��01wb��ʾ����Ƶ��00dc��00db
void RecordDlg::ParseMoviData(std::ifstream& file, char* id, unsigned int size)
{
	if (memcmp(id, "01wb", 4) == 0)				// ��Ƶ����
	{
		if (size % 2 != 0)
			size += 1;

		file.seekg(size, std::ios::cur);
		int mm = 0;
	}
	else if (memcmp(id, "00dc", 4) == 0)		// ѹ������Ƶ֡
	{
		if (size % 2 != 0)
			size += 1;

		file.seekg(size, std::ios::cur);
		int mm = 0;
	}
	else if (memcmp(id, "00db", 4) == 0)		// δѹ������Ƶ֡
	{
		if (size % 2 != 0)
			size += 1;

		file.seekg(size, std::ios::cur);
		int mm = 0;
	}
}

void RecordDlg::ReadMp4File()
{
	QString path = QString::fromLocal8Bit("D:/ý���ļ�/2023120310140.mp4");

	// �򿪶������ļ�
	std::ifstream file(QStr2Str(path), std::ios::binary);
	if (!file.is_open())
		return;

	// ��ȡ�ļ���С
	file.seekg(0, std::ios::end);
	std::size_t file_size = file.tellg();
	file.seekg(0, std::ios::beg);


}