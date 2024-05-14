#pragma once
#include <QDialog>
#include <QFutureWatcher>
#include "ui_voiceedit.h"
#include "public.h"

class Voice;
class VoiceEdit : public QDialog
{
	Q_OBJECT

public:
	VoiceEdit(QWidget *parent = Q_NULLPTR);
	~VoiceEdit();

private slots:
	void SelectFolder();
	void SpeakText();
	void FinishCurrentRun();
	void ConvertVoice();

signals:
	void Finish();

private:
	Ui::VoiceEdit ui;

	bool	m_bThreadRun;

	Voice*		m_pVoice;
	QFutureWatcher<void>*		m_pFutureWatcher;

	void VoiceSpeak(QString);
	void StartConvertVoice(QString);
};
