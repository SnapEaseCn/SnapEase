#include "imageview.h"
#include "arrowitem.h"
#include "cropitem.h"
#include "imagescene.h"
#include "penitem.h"
#include "textitem.h"
#include "textdlg.h"
#include "imageedit.h"
#include <QtConcurrent>
#include <QPainter>
#include <QMouseEvent>
#include <QGraphicsPixmapItem>
#include <QRectF>
#include <QClipboard>
#include <QApplication>
#include <QButtonGroup>
#include <QPainterPath>
#include <QDebug>
#include <functional>

ImageView::ImageView(QWidget *parent):QGraphicsView(parent)
{
	m_bMouseDown = false;
	m_PixmapItem = NULL;
	m_pCropItem = NULL;

	this->setBackgroundBrush(QBrush(QColor(16, 42, 57)));
	m_pScene = new ImageScene(this);
	QRectF viewRect(0, 0, width(), height());
	m_pScene->setSceneRect(viewRect);
	this->setScene(m_pScene);

	m_CurColor = QColor(255,0,0);
	m_CurDraw = OPER_MOVE;

	QBrush brush(QColor(16, 42, 57));
	m_pScene->setBackgroundBrush(brush);

	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

ImageView::~ImageView()
{
}

void ImageView::SetPixmap(QPixmap pixmap)
{
	while (stack_undo.size()>0)
	{
		QGraphicsItem* item = stack_undo.top();
		m_pScene->removeItem(item);

		delete item;
		item = NULL;

		stack_undo.pop();
	}
	while (stack_redo.size()>0)
	{
		QGraphicsItem* item = stack_undo.top();
		delete item;
		item = NULL;

		stack_undo.pop();
	}
	if (m_PixmapItem == NULL)
	{
		m_PixmapItem = new QGraphicsPixmapItem(pixmap);
	}
	else
	{
		m_PixmapItem->setPixmap(pixmap);
	}
	m_iImageWidth = m_PixmapItem->pixmap().width();
	m_iImageHeight = m_PixmapItem->pixmap().height();
	m_PixmapItem->setPos(0, 0);
	m_pScene->addItem(m_PixmapItem);

	FitImageToView();
}

void ImageView::FitImageToView()
{
	FitOriginImage();
	this->resetTransform();
}

void ImageView::FitOriginImage()
{
	QRectF viewRect(0, 0, this->width() - 10, this->height() - 10);

	if (m_PixmapItem != NULL)
	{
		//int width = m_PixmapItem->pixmap().width();
		//int height = m_PixmapItem->pixmap().height();
		int width = m_iImageWidth;
		int height = m_iImageHeight;

		int x=0, y=0;
		if (width < viewRect.width())
		{
			x = (viewRect.width() - width) / 2;
			y = (viewRect.height() - height) / 2;

			QPoint center(viewRect.width() / 2, viewRect.height() / 2);
			m_pScene->setSceneRect(QRect(QPoint(center.x() - width / 2, center.y() - height / 2), QPoint(center.x() + width / 2, center.y() + height / 2)));
		}
		else
		{
			m_pScene->setSceneRect(QRect(QPoint(0,0),QPoint(width,height)));
		}
		
		m_PixmapItem->setScale(1.0);
		m_PixmapItem->setPos(x, y);
	}
}

void ImageView::paintEvent(QPaintEvent *event)
{
	QGraphicsView::paintEvent(event);
}

void ImageView::mouseMoveEvent(QMouseEvent* evt)
{
	if (m_bMouseDown)
	{
		QPoint cur_point = evt->pos();
		if (m_CurDraw == DRAW_RECT)
		{
			QGraphicsRectItem* rectitem = (QGraphicsRectItem*)stack_undo.top();
			QPointF point0 = mapToScene(m_LastPos);
			QPointF point1 = mapToScene(cur_point);

			QPointF start_point(std::min(point0.x(), point1.x()), std::min(point0.y(), point1.y()));
			QPointF end_point(std::max(point0.x(), point1.x()), std::max(point0.y(), point1.y()));

			QRectF _rect = m_PixmapItem->mapRectFromScene(QRectF(start_point, end_point));
			rectitem->setRect(_rect);
		}
		else if (m_CurDraw == DRAW_ARRAW)
		{
			ArrowItem* item = (ArrowItem*)stack_undo.top();
			QPointF point = m_PixmapItem->mapFromScene(mapToScene(cur_point));
			item->SetEndPoint(point);
			item->update();
		}
		else if (m_CurDraw == DRAW_PEN)
		{
			PenItem* item = (PenItem*)stack_undo.top();
			QPointF point = m_PixmapItem->mapFromScene(mapToScene(cur_point));
			item->AddToPath(point);
			item->update();
		}
		else if (m_CurDraw == OPER_MOVE)
		{
			if (m_PixmapItem != NULL)
			{
				QPoint cur_point = evt->pos();
				QPointF ScenePoint = mapToScene(cur_point);
				QPointF delta = ScenePoint - m_PressPoint;
				m_PixmapItem->setPos(m_PixPos + delta);
			}
		}
	}
	QGraphicsView::mouseMoveEvent(evt);
}

void ImageView::mousePressEvent(QMouseEvent* evt)
{
	m_bMouseDown = true;
	m_LastPos = evt->pos();
	m_PressPoint = mapToScene(m_LastPos);

	if (m_CurDraw == DRAW_RECT)
	{
		QGraphicsRectItem* rectitem = new QGraphicsRectItem(m_PixmapItem);
		m_pScene->addItem(rectitem);
		rectitem->setPen(QPen(m_CurColor, 2));
		stack_undo.push(rectitem);
	}
	else if (m_CurDraw == DRAW_ARRAW)
	{
		ArrowItem* item = new ArrowItem(m_PixmapItem);
		QPointF start_point = m_PixmapItem->mapFromScene(m_PressPoint);
		item->SetStartPoint(start_point);
		item->SetColor(m_CurColor);
		item->SetBoundingRect(m_PixmapItem->boundingRect());
		m_pScene->addItem(item);
		stack_undo.push(item);
	}
	else if (m_CurDraw == DRAW_PEN)
	{
		PenItem* item = new PenItem(m_PixmapItem);
		QPointF start_point = m_PixmapItem->mapFromScene(m_PressPoint);
		item->AddNewPath(m_CurColor, start_point);
		item->SetBoundingRect(m_PixmapItem->boundingRect());
		m_pScene->addItem(item);
		stack_undo.push(item);
	}
	else if (m_CurDraw == DRAW_TEXT)
	{
		TextItem* item = new TextItem(m_PixmapItem);
		QPointF start_point = m_PixmapItem->mapFromScene(m_PressPoint);
		item->SetPosition(start_point);
		item->SetBoundingRect(m_PixmapItem->boundingRect());
		m_pScene->addItem(item);
		stack_undo.push(item);

		TextDlg* dlg = new TextDlg(this);

		auto func = std::bind(&ImageView::UpdateTextFont, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
		dlg->SetUpdateFont(func);
		dlg->SetColor(m_CurColor);

		auto exit_func = std::bind(&ImageView::ExitAddText, this);
		dlg->SetExitFun(exit_func);
		dlg->show();
	}
	else if (m_CurDraw == OPER_MOVE)
	{
		if (m_PixmapItem != NULL)
		{
			m_PixPos = m_PixmapItem->pos();
		}
	}
	QGraphicsView::mousePressEvent(evt);
}

void ImageView::mouseReleaseEvent(QMouseEvent* evt)
{
	m_bMouseDown = false;
	QGraphicsView::mouseReleaseEvent(evt);
}

void ImageView::CopyImageToClipBoard()
{
	if (m_PixmapItem != NULL && m_PixmapItem->pixmap().width() > 0)
	{
		QRectF scenerect(0,0,m_iImageWidth,m_iImageHeight);
		QImage image(scenerect.size().toSize(), QImage::Format_ARGB32);
		image.fill(Qt::transparent);
		QPainter painter(&image);
		m_pScene->render(&painter);
		painter.end();

		image = image.copy(scenerect.toRect());

		// 获取剪贴板
		QClipboard *clipboard = QApplication::clipboard();
		// 将QPixmap设置为剪贴板的图片数据
		clipboard->setPixmap(QPixmap::fromImage(image));
	}
}

void ImageView::ImageZoomIn()
{
	if (m_PixmapItem != NULL)
	{
		this->scale(0.8, 0.8);
	}
}

void ImageView::ImageZoomOut()
{
	if (m_PixmapItem != NULL)
	{
		this->scale(1.2, 1.2);
	}
}

void ImageView::wheelEvent(QWheelEvent* evt)
{
	int delta = evt->delta();
}

void ImageView::SetCurDrawType(int id)
{
	m_CurDraw = (DRAW_TYPE)id;
	m_bMouseDown = false;
}

void ImageView::SetCurColor(QColor clr)
{
	m_CurColor = clr;
}

void ImageView::GetImage(QImage*& img)
{
	if (m_PixmapItem != NULL && m_PixmapItem->pixmap().width() > 0)
	{
		QImage image(m_pScene->sceneRect().size().toSize(), QImage::Format_ARGB32);
		image.fill(Qt::transparent);
		QPainter painter(&image);
		m_pScene->render(&painter);
		painter.end();

		QRectF boundingRect = m_PixmapItem->mapRectToParent(m_PixmapItem->boundingRect());

		img = new QImage(image.copy(boundingRect.toRect()));
	}
}

void ImageView::UndoOper()
{
	if (stack_undo.size() > 0)
	{
		QGraphicsItem* item = stack_undo.top();
		m_pScene->removeItem(item);
		stack_undo.pop();

		stack_redo.push(item);
	}
}

void ImageView::RedoOper()
{
	if (stack_redo.size() > 0)
	{
		QGraphicsItem* item = stack_redo.top();
		m_pScene->addItem(item);
		item->setParentItem(m_PixmapItem);
		stack_redo.pop();
		stack_undo.push(item);
	}
}

void ImageView::ResizeImage(int width, int height)
{
	if (m_PixmapItem == NULL || m_iImageWidth == width || m_iImageHeight == height)
		return;

	int boundingWidth = m_PixmapItem->pixmap().width();
	int boundingHeight = m_PixmapItem->pixmap().height();

	// 计算缩放比例
	double widthScale = static_cast<double>(width) / boundingWidth;
	double heightScale = static_cast<double>(height) / boundingHeight;

	 // 创建缩放变换
	QTransform transform;
	transform.scale(widthScale, heightScale);

	m_iImageWidth = width;
	m_iImageHeight = height;

	 // 设置缩放变换
	m_PixmapItem->setTransform(transform);

	FitImageToView();
}

bool ImageView::GetImageSize(int& width, int& height)
{
	if (m_PixmapItem == NULL)
		return false;

	//width = m_PixmapItem->pixmap().width();
	//height = m_PixmapItem->pixmap().height();

	width = m_iImageWidth;
	height = m_iImageHeight;

	return true;
}

void ImageView::CropImage()
{
	if (m_PixmapItem == NULL)
		return;

	if (m_pCropItem == NULL)
	{
		m_pCropItem = new CropItem(m_PixmapItem);
	}
	m_pCropItem->SetBoundingRect(QRect(QPoint(0,0),QPoint(m_iImageWidth,m_iImageHeight)));
	m_pScene->addItem(m_pCropItem);

	m_CurDraw = DRAW_UNKNOW;
}

void ImageView::FinishCropImage()
{
	if (m_pCropItem != NULL)
	{
		QRectF rect = m_pCropItem->GetCropRect();
		if (rect != m_PixmapItem->boundingRect())
		{
			QPixmap pixmap = m_PixmapItem->pixmap().copy(rect.toRect());
			m_PixmapItem->setPixmap(pixmap);

			m_iImageWidth = rect.width();
			m_iImageHeight = rect.height();
		}

		m_pScene->removeItem(m_pCropItem);
		delete m_pCropItem;
		m_pCropItem = NULL;

		FitImageToView();
	}
}

void ImageView::keyPressEvent(QKeyEvent* evt)
{
	int key = evt->key()+1;
	if (key == Qt::Key_Enter)
	{
		FinishCropImage();
	}
	else if (key == Qt::Key_Escape)
	{
		if (m_pCropItem != NULL)
		{
			m_pScene->removeItem(m_pCropItem);
			delete m_pCropItem;
			m_pCropItem = NULL;
		}
	}
}

void ImageView::UpdateTextFont(QString text,QColor clr, QFont font, int size)
{
	TextItem* item = (TextItem*)stack_undo.top();
	item->SetText(text, clr, font, size);
	item->update();
}

void ImageView::ExitAddText()
{
	ImageEdit* edit = (ImageEdit*)parent();
	edit->ExitAddText();
}