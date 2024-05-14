#include "QrCode.h"
#include "qrencode.h"
#include <QtConcurrent>
#include <QColorDialog>
#include "text.h"
#include <QFileDialog>
#include <QPainter>

#define COLOR_SIZE 8

QrCode::QrCode(QWidget *parent):QDialog(parent)
{
	ui.setupUi(this);
	Qt::WindowFlags flags = windowFlags();
	//flags |= Qt::WindowMinMaxButtonsHint;
	flags |= Qt::WindowCloseButtonHint;
	this->setWindowFlags(flags);

	codeptr = NULL;

	ui.boxExportPixel->setItemData(0, QVariant(40));
	ui.boxExportPixel->setItemData(1, QVariant(80));
	ui.boxExportPixel->setItemData(2, QVariant(160));
	ui.boxExportPixel->setItemData(3, QVariant(320));
	ui.boxExportPixel->setItemData(4, QVariant(640));
	ui.boxExportPixel->setItemData(5, QVariant(1280));
	ui.boxExportPixel->setItemData(6, QVariant(2048));
	
	connect(ui.tbColorPick, &QPushButton::clicked, this, &QrCode::SelectColor);
	connect(ui.pbGeneral, &QPushButton::clicked, this, &QrCode::GeneralQrCode);
	connect(ui.pbSaveImage, &QPushButton::clicked, this, &QrCode::SaveImage);

	colors.resize(COLOR_SIZE);
	colors[0] = QColor(0, 0, 0);
	colors[1] = QColor(23, 96, 121);
	colors[2] = QColor(28, 174, 94);
	colors[3] = QColor(239, 157, 40);
	colors[4] = QColor(197, 54, 20);
	colors[5] = QColor(222, 160, 9);
	colors[6] = QColor(196, 141, 37);
	colors[7] = QColor(131, 96, 62);

	m_CurClr = colors[0];
	for (short i = 0; i < COLOR_SIZE; i++)
	{
		QString name = "tbColor" + QString::number(i);
		QToolButton* btn_color = this->findChild<QToolButton*>(name);
		if (btn_color != NULL)
		{
			btn_color->setProperty("id", i);
			QColor clr = colors[i];
			QString qss = QString("border:none;background-color:rgb(%0,%1,%2)").arg(clr.red()).arg(clr.green()).arg(clr.blue());
			btn_color->setStyleSheet(qss);
		}
		connect(btn_color, &QToolButton::clicked, this, &QrCode::ColorClicked);
	}
}

QrCode::~QrCode()
{
	if (codeptr != NULL)
	{
		QRcode_free((QRcode*)codeptr);
		codeptr = NULL;
	}
}

void QrCode::GeneralQrCode()
{
	QString text = ui.textEdit->toPlainText().trimmed();
	if (!text.isEmpty())
	{
		int version = ui.comboBoxVersion->currentIndex()+2;
		int level = ui.comboBoxLevel->currentIndex();

		if (codeptr != NULL)
		{
			QRcode_free((QRcode*)codeptr);
			codeptr = NULL;
		}
		// 使用 qrencode 生成二维码
		codeptr = QRcode_encodeString(text.toUtf8().constData(), version, (QRecLevel)level, QR_MODE_8, 1);
		if (codeptr != NULL)
		{
			ShowCodeImage();
		}		
	}
}

void QrCode::ShowCodeImage()
{
	if (codeptr != NULL)
	{
		QRcode* ptr = (QRcode*)codeptr;
		// 将二维码转换为 QImage
		QImage image(ptr->width, ptr->width, QImage::Format_RGB32);
		for (int y = 0; y < ptr->width; y++)
		{
			for (int x = 0; x < ptr->width; x++)
			{
				int color = ptr->data[y * ptr->width + x] & 1;
				image.setPixel(x, y, color ? qRgb(m_CurClr.red(), m_CurClr.green(), m_CurClr.blue()): qRgb(255, 255, 255));
			}
		}

		image = image.scaled(160, 160, Qt::KeepAspectRatioByExpanding, Qt::FastTransformation);
		ui.label_img->setPixmap(QPixmap::fromImage(image));
	}
}

void QrCode::ColorClicked()
{
	QToolButton* btn = (QToolButton*)sender();
	int id = btn->property("id").toInt();
	if (colors.size() > id)
	{
		m_CurClr = colors[id];
		ShowCodeImage();
	}
}

void QrCode::SelectColor()
{
	QColorDialog dialog(this);
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
	m_CurClr = dialog.getColor();
	ShowCodeImage();
}

void QrCode::SaveImage()
{
	if (codeptr == NULL)
		return;

	QString filter = Str2QStr("PNG Format(*.png);;JPG Format(*.jpg;*.jpeg);;BMP Format(*.bmp);;TIF Format(*.tif;*.tiff);;All Files (*)");
	std::string path = GenerateTimeStr();
	// 创建文件对话框
	QString filePath = QFileDialog::getSaveFileName(this, tr("Save image"), Str2QStr("/path/to/default/save/location/") + Str2QStr(path), filter);
	
	if (!filePath.isEmpty())
	{
		int value = ui.boxExportPixel->currentData().toInt();
		
		QRcode* ptr = (QRcode*)codeptr;
		// 将二维码转换为 QImage
		QImage image(ptr->width, ptr->width, QImage::Format_RGB32);
		for (int y = 0; y < ptr->width; y++)
		{
			for (int x = 0; x < ptr->width; x++)
			{
				int color = ptr->data[y * ptr->width + x] & 1;
				image.setPixel(x, y, color ? qRgb(m_CurClr.red(), m_CurClr.green(), m_CurClr.blue()) : qRgb(255, 255, 255));
			}
		}
		image = addBorderToImage(image,2);
		image = image.scaled(value, value, Qt::KeepAspectRatioByExpanding, Qt::FastTransformation);
		image.save(filePath);
	}
}

void QrCode::closeEvent(QCloseEvent *event)
{
	this->deleteLater();
	QDialog::closeEvent(event);
}

QImage QrCode::addBorderToImage(const QImage& original, int borderWidth) 
{
	// Create a new image with increased dimensions
	QImage bordered(original.width() + 2 * borderWidth, original.height() + 2 * borderWidth, QImage::Format_ARGB32);

	// Fill the new image with white color
	bordered.fill(Qt::white);

	// Create a QPainter to draw the original image onto the new image
	QPainter painter(&bordered);
	painter.drawImage(borderWidth, borderWidth, original);

	return bordered;
}