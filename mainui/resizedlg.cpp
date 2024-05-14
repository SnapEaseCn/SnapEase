#include "ResizeDlg.h"
#include <QtConcurrent>

ResizeDlg::ResizeDlg(QWidget *parent):QDialog(parent)
{
	ui.setupUi(this);
	Qt::WindowFlags flags = windowFlags();
	flags |= Qt::WindowMinMaxButtonsHint;
	flags |= Qt::WindowCloseButtonHint;
	this->setWindowFlags(flags);

	ui.checkBox->setFocus();

	connect(ui.spinBoxHeight, SIGNAL(editingFinished()), this, SLOT(HeightChange()));
	connect(ui.spinBoxWidth, SIGNAL(editingFinished()), this, SLOT(WidthChange()));

	connect(ui.pbOK, &QPushButton::clicked, this, &ResizeDlg::FinishDone);
	connect(ui.pbCancel, &QPushButton::clicked, this, &ResizeDlg::reject);
}

ResizeDlg::~ResizeDlg()
{
}

void ResizeDlg::GetImageSize(unsigned int& width, unsigned int& height)
{
	width = ui.spinBoxWidth->value();
	height = ui.spinBoxHeight->value();
}

void ResizeDlg::HeightChange()
{
	int height = ui.spinBoxHeight->value();
	if (m_iLastHeight == height)
		return;

	bool check = ui.checkBox->isChecked();
	if (check)
	{
		double rate = (double)height/m_iHeight;
		m_iLastWidth = m_iWidth * rate;
		ui.spinBoxWidth->setValue(m_iLastWidth);
	}
	m_iLastHeight = height;
}

void ResizeDlg::WidthChange()
{
	int width = ui.spinBoxWidth->value();
	if (m_iLastWidth == width)
		return;

	bool check = ui.checkBox->isChecked();
	if (check)
	{
		double rate = (double)width/m_iWidth ;
		m_iLastHeight = m_iHeight * rate;
		ui.spinBoxHeight->setValue(m_iLastHeight);
	}
	m_iLastWidth = width;
}

void ResizeDlg::FinishDone()
{
	int width = ui.spinBoxWidth->value();
	int height = ui.spinBoxHeight->value();
	emit FinishResize(width, height);
	this->accept();
}

void ResizeDlg::SetImageSize(unsigned int width, unsigned int height)
{
	m_iWidth = width;
	m_iHeight = height; 

	ui.spinBoxWidth->setValue(m_iWidth);
	ui.spinBoxHeight->setValue(m_iHeight);
}