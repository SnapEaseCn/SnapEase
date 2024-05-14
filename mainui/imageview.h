#pragma once

#include <QGraphicsView>
#include "public.h"
#include <stack>

class CropItem;
class ImageScene;
class ImageView : public QGraphicsView
{
public:
	ImageView(QWidget *parent = Q_NULLPTR);
	~ImageView();

	void GetImage(QImage*& img);
	void SetPixmap(QPixmap pixmap);
	void CopyImageToClipBoard();
	void ImageZoomIn();
	void ImageZoomOut();
	void FitImageToView();
	void FitOriginImage();
	void SetCurDrawType(int id);
	void SetCurColor(QColor clr);
	void UndoOper();
	void RedoOper();
	bool GetImageSize(int&,int&);
	void ResizeImage(int width, int height);
	void CropImage();
	void FinishCropImage();
	void keyPressEvent(QKeyEvent* event) override;
	void UpdateTextFont(QString,QColor, QFont, int);
	void ExitAddText();

private:
	bool			m_bMouseDown;
	QColor			m_CurColor;
	QPoint			m_LastPos;
	QPointF			m_PixPos;
	QPointF			m_PressPoint;
	DRAW_TYPE		m_CurDraw;
	int				m_iImageWidth;
	int				m_iImageHeight;

	ImageScene*				m_pScene;
	QGraphicsPixmapItem*	m_PixmapItem;
	CropItem*				m_pCropItem;

	std::stack<QGraphicsItem*>	stack_undo;
	std::stack<QGraphicsItem*>	stack_redo;

	void paintEvent(QPaintEvent* evt);
	void mouseMoveEvent(QMouseEvent* evt);
	void mousePressEvent(QMouseEvent* evt);
	void mouseReleaseEvent(QMouseEvent* evt);
	void wheelEvent(QWheelEvent* evt);
};
