#include "imageedit.h"
#include "imagereader.h"
#include "resizedlg.h"
#include <QtConcurrent>
#include <QGraphicsPixmapItem>
#include <QRectF>
#include <QClipboard>
#include <QMouseEvent>
#include <QImage>
#include <QFileDialog>
#include <QButtonGroup>
#include <QColorDialog>
#include "text.h"

ImageEdit::ImageEdit(QWidget *parent):QDialog(parent)
{
	ui.setupUi(this);

	Qt::WindowFlags flags = windowFlags(); 
	flags |= Qt::WindowMinMaxButtonsHint; 
	flags |= Qt::WindowCloseButtonHint; 
	this->setWindowFlags(flags); 
	setFocusPolicy(Qt::StrongFocus);

	m_Image = NULL;

	connect(ui.tbOpen, &QToolButton::clicked, this, &ImageEdit::OpenImage);
	connect(ui.tbSave, &QToolButton::clicked, this, &ImageEdit::SaveImage);
	connect(ui.tbCopy, &QToolButton::clicked, this, &ImageEdit::CopyImageToClipBoard);
	connect(ui.tbZoom, &QToolButton::clicked, this, &ImageEdit::FitImageToView);
	connect(ui.tbZoomIn, &QToolButton::clicked, this, &ImageEdit::ImageZoomIn);
	connect(ui.tbZoomOut, &QToolButton::clicked, this, &ImageEdit::ImageZoomOut);
	connect(ui.tbSelColor, &QToolButton::clicked, this, &ImageEdit::SelectColor);
	connect(ui.tbUndo, &QToolButton::clicked, this, &ImageEdit::UndoOper);
	connect(ui.tbRedo, &QToolButton::clicked, this, &ImageEdit::RedoOper);
	connect(ui.tbResize, &QToolButton::clicked, this, &ImageEdit::ResizeImage);
	connect(ui.tbCrop, &QToolButton::clicked, this, &ImageEdit::CropImage);

	connect(this, SIGNAL(rejected()), this, SIGNAL(FinishEdit()));

	m_pFutureWatcher = new QFutureWatcher<void>;
	connect(m_pFutureWatcher, SIGNAL(finished()), this, SLOT(FinishCurrentRun()));

	m_group = new QButtonGroup(this);
	m_group->addButton(ui.tbMove, OPER_MOVE);
	m_group->addButton(ui.tbText, DRAW_TEXT);
	m_group->addButton(ui.tbArrow, DRAW_ARRAW);
	m_group->addButton(ui.tbPen, DRAW_PEN);
	m_group->addButton(ui.tbRect, DRAW_RECT);
	connect(m_group, SIGNAL(buttonClicked(int)), this, SLOT(SetCurDrawType(int)));

	QColor clr(255,0,0);
	ui.graphicsView->SetCurColor(clr);
	QString style = QString("border:1px solid rgb(16,42,57);background-color: rgb(%0, %1, %2);").arg(clr.red()).arg(clr.green()).arg(clr.blue());
	ui.tbSelColor->setStyleSheet(style);
}

ImageEdit::~ImageEdit()
{
	if (m_Image != NULL)
	{
		delete m_Image;
		m_Image = NULL;
	}
}

void ImageEdit::SetFilePath(QString filePath)
{
	QFuture<void> future = QtConcurrent::run(this, &ImageEdit::OpenFile, filePath);
	m_pFutureWatcher->setFuture(future);
}

void ImageEdit::OpenFile(QString path)
{
	if (m_Image == NULL)
	{
		m_Image = new QImage();
	}
	if (!m_Image->load(path))
	{
		delete m_Image;
		m_Image = NULL;
	}
}

void ImageEdit::SetPixmap(QPixmap pixmap)
{
	ui.graphicsView->SetPixmap(pixmap);
	ui.graphicsView->FitOriginImage();
}

void ImageEdit::FitImageToView()
{
	ui.graphicsView->FitImageToView();
}

void ImageEdit::resizeEvent(QResizeEvent* evt)
{
	ui.graphicsView->FitOriginImage();
}

void ImageEdit::CopyImageToClipBoard()
{
	ui.graphicsView->CopyImageToClipBoard();
	this->FinishEdit();
}

void ImageEdit::ImageZoomIn()
{
	ui.graphicsView->ImageZoomIn();
}

void ImageEdit::ImageZoomOut()
{
	ui.graphicsView->ImageZoomOut();
}

void ImageEdit::FinishCurrentRun()
{
	if (m_Image != NULL)
	{
		ui.graphicsView->SetPixmap(QPixmap::fromImage(*m_Image));
	}
}

void ImageEdit::OpenImage()
{
	QString path = SelectFileNamePath();
	if (!path.isEmpty())
	{
		QFuture<void> future = QtConcurrent::run(this, &ImageEdit::OpenFile, path);
		m_pFutureWatcher->setFuture(future);
	}
}

void ImageEdit::SaveImage()
{
	QString path = SelectSaveFile();
	if (!path.isEmpty())
	{
		QFuture<void> future = QtConcurrent::run(this, &ImageEdit::SaveFile, path);
	}
}

QString ImageEdit::SelectFileNamePath()
{
	QString fileName;
	QFileDialog file_dlg;
	file_dlg.setWindowTitle(tr("Select file"));
	file_dlg.setAcceptMode(QFileDialog::AcceptOpen);
	file_dlg.setFileMode(QFileDialog::AnyFile);
	file_dlg.setViewMode(QFileDialog::Detail);

	QStringList filters;
	filters << tr("PNG Format(*.png)") << tr("JPG Format(*.jpg;*.jpeg)") << tr("BMP Format(*.bmp)") << tr("TIF Format(*.tif;*.tiff)")<< tr("All Files(*)");
	file_dlg.setNameFilters(filters);

	file_dlg.setGeometry(10, 30, 300, 200);
	file_dlg.setDirectory(Str2QStr("."));
	if (file_dlg.exec() == QDialog::Accepted)
	{
		fileName = file_dlg.selectedFiles()[0];
	}
	return fileName;
}

QString ImageEdit::SelectSaveFile()
{
	QString filter = Str2QStr("PNG Format(*.png);;JPG Format(*.jpg;*.jpeg);;BMP Format(*.bmp);;TIF Format(*.tif;*.tiff);;All Files (*)");
	std::string path = GenerateTimeStr();
	// 创建文件对话框
	QString filePath = QFileDialog::getSaveFileName(nullptr, tr("Save image"), Str2QStr("/path/to/default/save/location/")+ Str2QStr(path), filter);
	return filePath;
}

void ImageEdit::SaveFile(QString path)
{
	QImage* m_Image = NULL;
	ui.graphicsView->GetImage(m_Image);
	if (m_Image != NULL && !path.isEmpty())
	{
		m_Image->save(path);
	}
}

void ImageEdit::SetCurDrawType(int id)
{
	ui.graphicsView->SetCurDrawType(id);
}

void ImageEdit::SelectColor()
{
	QColorDialog dialog;
	dialog.setOption(QColorDialog::ShowAlphaChannel); // 启用透明度选项
	// 其他自定义设置
	dialog.setCustomColor(0,QColor(168, 200, 223));
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
	ui.graphicsView->SetCurColor(clr);
	// 处理选定的颜色
	QString style = QString("border:1px solid rgb(16,42,57);background-color: rgb(%0, %1, %2);").arg(clr.red()).arg(clr.green()).arg(clr.blue());
	ui.tbSelColor->setStyleSheet(style);
}

void ImageEdit::UndoOper()
{
	ui.graphicsView->UndoOper();
}

void ImageEdit::RedoOper()
{
	ui.graphicsView->RedoOper();
}

void ImageEdit::ResizeImage()
{
	int width, height;
	bool ret = ui.graphicsView->GetImageSize(width, height);
	if (ret)
	{
		ResizeDlg dlg(this);
		dlg.SetImageSize(width, height);
		connect(&dlg, &ResizeDlg::FinishResize, this, &ImageEdit::DoneResizeImage);
		dlg.exec();
	}
}

void ImageEdit::DoneResizeImage(int width, int height)
{
	ui.graphicsView->ResizeImage(width,height);
}

void ImageEdit::CropImage()
{
	ui.graphicsView->CropImage();
}

void ImageEdit::keyPressEvent(QKeyEvent* event)
{
	ui.graphicsView->keyPressEvent(event);
}

void ImageEdit::ExitAddText()
{
	m_group->button(OPER_MOVE)->setChecked(true);
	ui.graphicsView->SetCurDrawType(OPER_MOVE);
}