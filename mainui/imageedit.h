#pragma once

#include <QDialog>
#include <QGraphicsScene>
#include "ui_imageedit.h"
#include "public.h"
#include <QFutureWatcher>

class QResizeEvent;
class QGraphicsPixmapItem;
class QImage;
class ImageEdit : public QDialog
{
	Q_OBJECT

public:
	ImageEdit(QWidget *parent = Q_NULLPTR);
	~ImageEdit();

	void SetFilePath(QString filePath);
	void SetPixmap(QPixmap pixmap);
	void keyPressEvent(QKeyEvent* event)override;
	void ExitAddText();

signals:
	void FinishEdit();

private slots:
	void CopyImageToClipBoard();
	void FitImageToView();
	void ImageZoomIn();
	void ImageZoomOut();
	void FinishCurrentRun();
	void OpenImage();
	void SaveImage();
	void SetCurDrawType(int);
	void SelectColor();
	void UndoOper();
	void RedoOper();
	void ResizeImage();
	void DoneResizeImage(int,int);
	void CropImage();

private:
	Ui::ImageEdit ui;
	QFutureWatcher<void>*		m_pFutureWatcher;
	QImage*				m_Image;

	QButtonGroup*		m_group;

	void resizeEvent(QResizeEvent* evt);
	void OpenFile(QString path);
	void SaveFile(QString path);
	QString SelectFileNamePath();
	QString SelectSaveFile();
};
