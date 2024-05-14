#pragma once

#include <QDialog>
#include "ui_recorddlg.h"
#include "public.h"

class RecordDlg : public QDialog
{
	Q_OBJECT

public:
	RecordDlg(QWidget *parent = Q_NULLPTR);
	~RecordDlg();

private slots:
	void StartRecord();
	void FinishDrawRect(QRect);

private:
	Ui::RecordDlg ui;

	QTimer*		m_timer;

	void ReadAviDemo();
	void ReadAviChunk(std::ifstream& file,char* id,int size);
	void ParseMoviData(std::ifstream& file, char* id,unsigned int len);

	void ReadMp4File();
};
