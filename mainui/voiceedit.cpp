#include "voiceedit.h"
#include "voice.h"
#include "glob.h"
#include "text.h"
#include <QtConcurrent>
#include <QFileDialog>

VoiceEdit::VoiceEdit(QWidget *parent):QDialog(parent)
{
	ui.setupUi(this);

	Qt::WindowFlags flags = windowFlags();
	flags |= Qt::WindowMinMaxButtonsHint;
	flags |= Qt::WindowCloseButtonHint;
	this->setWindowFlags(flags);

	m_pVoice = new Voice();
	std::map<int, QString> availables = m_pVoice->GetVoiceAvailable();
	for (const auto& itr : availables)
	{
		int idx = itr.first;
		QString name = itr.second;
		ui.comboBoxAudio->addItem(name, idx);
	}

	connect(ui.tbClose, SIGNAL(clicked()), this, SIGNAL(Finish()));
	connect(ui.tbSelectFolder, SIGNAL(clicked()), this, SLOT(SelectFolder()));
	connect(this, SIGNAL(rejected()), this, SIGNAL(Finish()));
	connect(ui.tbTryListen, SIGNAL(clicked()), this, SLOT(SpeakText()));
	connect(ui.tbConvert, SIGNAL(clicked()), this, SLOT(ConvertVoice()));

	std::string name = GLOB_INS->GetExportVoicePath()+GenerateTimeStr()+".wav";
	ui.lineEditPath->setText(Str2QStr(name));

	ui.textEdit->setText(QString::fromLocal8Bit("Enter the text you want to convert into speech."));

	m_pFutureWatcher = new QFutureWatcher<void>;
	connect(m_pFutureWatcher, SIGNAL(finished()), this, SLOT(FinishCurrentRun()));
	m_bThreadRun = false;
}

VoiceEdit::~VoiceEdit()
{
	if (m_pVoice != NULL)
	{
		delete m_pVoice;
		m_pVoice = NULL;
	}
}

void VoiceEdit::SelectFolder()
{
	QString fileName;
	QFileDialog file_dlg(this);
	file_dlg.setWindowTitle(tr("Select save path"));
	file_dlg.setAcceptMode(QFileDialog::AcceptSave);
	file_dlg.setFileMode(QFileDialog::AnyFile);
	QStringList filters;
	filters << tr("(*.wav)");
	file_dlg.setNameFilters(filters);
	file_dlg.setViewMode(QFileDialog::Detail);

	file_dlg.setGeometry(10, 30, 300, 200);
	file_dlg.setDirectory(Str2QStr("."));
	if (file_dlg.exec() == QDialog::Accepted)
	{
		fileName = file_dlg.selectedFiles()[0];
	}
	if (!fileName.isEmpty())
	{
		ui.lineEditPath->setText(fileName);
		std::string folder = std::string(fileName.toLocal8Bit());

		GLOB_INS->SetExportVoicePath(RemoveFilePart(folder));
		GLOB_INS->SaveConfigData();
	}
}

void VoiceEdit::SpeakText()
{
	QString text = ui.textEdit->toPlainText();
	if (!text.trimmed().isEmpty() && !m_bThreadRun)
	{
		QFuture<void> future = QtConcurrent::run(this, &VoiceEdit::VoiceSpeak, text);
		m_pFutureWatcher->setFuture(future);
		m_bThreadRun = true;
	}
}

void VoiceEdit::VoiceSpeak(QString text)
{
	m_pVoice->SpeakWinAPI(text);
	//m_pVoice->SpeakQtAPI(text);
}

void VoiceEdit::FinishCurrentRun()
{
	m_bThreadRun = false;
	ui.labelTip->setText(tr("Converting finish!"));
}

void VoiceEdit::ConvertVoice()
{
	QString text = ui.textEdit->toPlainText();
	if (!text.trimmed().isEmpty() && !m_bThreadRun)
	{
		QFuture<void> future = QtConcurrent::run(this, &VoiceEdit::StartConvertVoice, text);
		m_pFutureWatcher->setFuture(future);
		m_bThreadRun = true;
		ui.labelTip->setText(tr("Converting..."));
	}
}

void VoiceEdit::StartConvertVoice(QString text)
{
	QString path = ui.lineEditPath->text();
	m_pVoice->ConvertWAVFileWinApi(path,text);
}