#include "TextDlg.h"
#include <QtConcurrent>
#include <QColorDialog>

TextDlg::TextDlg(QWidget *parent):QDialog(parent)
{
	ui.setupUi(this);
	Qt::WindowFlags flags = windowFlags();
	flags |= Qt::WindowCloseButtonHint;
	this->setWindowFlags(flags);

	setModal(true);
	connect(ui.pbUpdate, &QPushButton::clicked, this, &TextDlg::UpdateFont);
	connect(ui.pbClose, &QPushButton::clicked, this, &TextDlg::close);
	connect(ui.tbColor, &QPushButton::clicked, this, &TextDlg::SelectColor);
}

TextDlg::~TextDlg()
{
}

void TextDlg::UpdateFont()
{
	QString text = ui.textEdit->toPlainText();
	QFont font = ui.fontComboBox->font();
	int size = ui.spinBox->value();
	UPDATE_FONT(text, m_color, font, size);
}

void TextDlg::closeEvent(QCloseEvent* evt)
{
	EXIT_TEXT();
	this->deleteLater();
}

void TextDlg::SelectColor()
{
	QColorDialog dialog;
	dialog.setOption(QColorDialog::ShowAlphaChannel); // 启用透明度选项
	// 其他自定义设置
	dialog.setCustomColor(0, QColor(168, 200, 223));
	dialog.setCustomColor(1, QColor(106, 198, 221));
	dialog.setCustomColor(2, QColor(38, 111, 166));
	dialog.setCustomColor(3, QColor(43, 56, 111));
	dialog.setCustomColor(4, QColor(128, 125, 168));
	dialog.setCustomColor(5, QColor(38, 154, 195));
	dialog.setCustomColor(6, QColor(27, 92, 158));
	dialog.setCustomColor(7, QColor(143, 201, 223));

	dialog.setCustomColor(8, QColor(253, 230, 128));
	dialog.setCustomColor(9, QColor(251, 141, 78));
	dialog.setCustomColor(10, QColor(198, 93, 87));
	dialog.setCustomColor(11, QColor(188, 111, 93));
	dialog.setCustomColor(12, QColor(131, 96, 74));
	dialog.setCustomColor(13, QColor(254, 228, 71));
	dialog.setCustomColor(14, QColor(223, 43, 70));
	dialog.setCustomColor(15, QColor(226, 74, 51));

	QColor clr = dialog.getColor();
	// 处理选定的颜色
	QString style = QString("border:none;background-color: rgb(%0, %1, %2);").arg(clr.red()).arg(clr.green()).arg(clr.blue());
	ui.tbColor->setStyleSheet(style);

	m_color = clr;
}