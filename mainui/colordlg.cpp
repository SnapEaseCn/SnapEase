#include "ColorDlg.h"
#include <QtConcurrent>
#include <QColor>
#include <QClipboard>
#include "text.h"

ColorDlg::ColorDlg(QWidget *parent):QDialog(parent)
{
	ui.setupUi(this);
	Qt::WindowFlags flags = windowFlags();
	flags |= Qt::WindowCloseButtonHint;
	this->setWindowFlags(flags);

	ui.tbRGB->setProperty("id", 0);
	ui.tbDEC->setProperty("id", 1);
	ui.tbHex->setProperty("id", 2);
	connect(ui.tbRGB, &QToolButton::clicked, this, &ColorDlg::CopyColor);
	connect(ui.tbHex, &QToolButton::clicked, this, &ColorDlg::CopyColor);
	connect(ui.tbDEC, &QToolButton::clicked, this, &ColorDlg::CopyColor);

	connect(ui.tbClose, &QToolButton::clicked, this, &QDialog::accept);
	connect(ui.tbPickOther, &QToolButton::clicked, this, &ColorDlg::PickOtherColor);
}

ColorDlg::~ColorDlg()
{
}

void ColorDlg::SetColor(int r, int g, int b)
{
	QString style = QString(Str2QStr("background-color: rgb(%0,%1,%2);")).arg(r).arg(g).arg(b);
	ui.frame->setStyleSheet(style);

	QString strRgb = QString(Str2QStr("%0,%1,%2")).arg(r).arg(g).arg(b);
	ui.lineEditRgb->setText(strRgb);

	QColor color = QColor::fromRgb(r,g,b);
	ui.lineEditHex->setText(color.name().toUpper());

	int hsv[4];
	color.getHsv(hsv, hsv+1, hsv+2);
	QString strDec = QString(Str2QStr("%1,%2,%3")).arg(hsv[0]).arg(hsv[1]).arg(hsv[2]);
	ui.lineEditDEC->setText(strDec);
}

void ColorDlg::CopyColor()
{
	int id = ((QToolButton*)sender())->property("id").toInt();
	// 创建一个QClipboard对象
	QClipboard *clipboard = QApplication::clipboard();
	if (id == 0)
	{
		// 将文本设置到剪贴板
		clipboard->setText(ui.lineEditRgb->text());
	}
	else if (id == 1)
	{
		// 将文本设置到剪贴板
		clipboard->setText(ui.lineEditDEC->text());
	}
	else if (id == 2)
	{
		// 将文本设置到剪贴板
		clipboard->setText(ui.lineEditHex->text());
	}
	emit  FinishEdit(false);
}

void ColorDlg::closeEvent(QCloseEvent* evt)
{
	emit  FinishEdit(false);
}

void ColorDlg::PickOtherColor()
{
	emit  FinishEdit(true);
}