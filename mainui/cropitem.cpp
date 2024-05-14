#include "CropItem.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneHoverEvent>
#include <QCursor>

#define HANDLENUM 8
#define HANDLESIZE 3

CropItem::CropItem(QGraphicsItem *parent):QGraphicsItem(parent)
{
	setFlag(QGraphicsItem::ItemIsSelectable);
	setAcceptHoverEvents(true);
	m_iSelHandleIdx = -1;
}

QRectF CropItem::boundingRect() const
{
	return m_BoundingRect;
}

QRectF CropItem::GetCropRect()
{
	return m_CurCropRect;
}

void CropItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

	painter->setPen(QColor(26, 173, 25));
	painter->drawRect(m_CurCropRect);

	painter->setBrush(QColor(26, 173, 25));
	for (int ii = 0; ii < HANDLENUM; ii++)
	{
		painter->drawEllipse(handles[ii]);
	}
}

void CropItem::SetBoundingRect(QRectF rect) 
{ 
	m_BoundingRect = QRectF(rect.topLeft()-QPointF(HANDLESIZE, HANDLESIZE), rect.bottomRight()+QPointF(HANDLESIZE, HANDLESIZE));
	m_CurCropRect = rect;

	handles.resize(HANDLENUM);
	for (int i = 0; i < HANDLENUM; ++i)
	{
		QRectF	rect(QPoint(0,0), QSizeF(HANDLESIZE*2, HANDLESIZE*2));
		handles[i] = rect;
	}
	UpdateHandles();
}

void CropItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	m_iSelHandleIdx = -1;
	m_dragStartPos = event->pos();
	for (int i = 0; i < HANDLENUM; i++)
	{
		if (handles[i].contains(m_dragStartPos))
		{
			m_iSelHandleIdx = i;
		}
	}	
	QGraphicsItem::mousePressEvent(event);
}

void CropItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	QGraphicsItem::mouseReleaseEvent(event);
}

void CropItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	QPointF curpoint = event->pos();
	if (m_BoundingRect.contains(curpoint))
	{
		if (m_iSelHandleIdx == 0)
		{
			m_CurCropRect.setTopLeft(curpoint);

			// 更新手柄位置
			UpdateHandles();
			update();
		}
		else if (m_iSelHandleIdx == 1)
		{
			m_CurCropRect.setTop(curpoint.y());
			// 更新手柄位置
			UpdateHandles();
			update();
		}
		else if (m_iSelHandleIdx == 2)
		{
			m_CurCropRect.setTopRight(curpoint);
			// 更新手柄位置
			UpdateHandles();
			update();
		}
		else if (m_iSelHandleIdx == 3)
		{
			m_CurCropRect.setRight(curpoint.x());
			// 更新手柄位置
			UpdateHandles();
			update();
		}
		else if (m_iSelHandleIdx == 4)
		{
			m_CurCropRect.setBottomRight(curpoint);
			// 更新手柄位置
			UpdateHandles();
			update();
		}
		else if (m_iSelHandleIdx == 5)
		{
			m_CurCropRect.setBottom(curpoint.y());
			// 更新手柄位置
			UpdateHandles();
			update();
		}
		else if (m_iSelHandleIdx == 6)
		{
			m_CurCropRect.setBottomLeft(curpoint);
			// 更新手柄位置
			UpdateHandles();
			update();
		}
		else if (m_iSelHandleIdx == 7)
		{
			m_CurCropRect.setLeft(curpoint.x());
			// 更新手柄位置
			UpdateHandles();
			update();
		}
	}
	QGraphicsItem::mouseMoveEvent(event);
}

void CropItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
	QPointF pos = event->pos();
	if (handles[0].contains(pos) || handles[4].contains(pos))
	{
		setCursor(Qt::SizeFDiagCursor);
	}
	else if (handles[1].contains(pos) || handles[5].contains(pos))
	{
		setCursor(Qt::SizeVerCursor);
	}
	else if (handles[2].contains(pos) || handles[6].contains(pos))
	{
		setCursor(Qt::SizeBDiagCursor);
	}
	else if (handles[3].contains(pos) || handles[7].contains(pos))
	{
		setCursor(Qt::SizeHorCursor);
	}
	else
	{
		setCursor(Qt::ArrowCursor);
	}
}

void CropItem::UpdateHandles()
{
	handles[0] = QRectF(m_CurCropRect.topLeft()-QPoint(HANDLESIZE, HANDLESIZE),QSizeF(HANDLESIZE*2, HANDLESIZE*2));
	handles[1] = QRectF(m_CurCropRect.topLeft()+QPointF(m_CurCropRect.width()/2,-HANDLESIZE), QSizeF(HANDLESIZE * 2, HANDLESIZE * 2));
	handles[2] = QRectF(m_CurCropRect.topRight()-QPoint(HANDLESIZE, HANDLESIZE), QSizeF(HANDLESIZE * 2, HANDLESIZE * 2));
	handles[3] = QRectF(m_CurCropRect.topRight()+QPointF(-HANDLESIZE,m_CurCropRect.height()/2), QSizeF(HANDLESIZE * 2, HANDLESIZE * 2));
	handles[4] = QRectF(m_CurCropRect.bottomRight()- QPoint(HANDLESIZE, HANDLESIZE), QSizeF(HANDLESIZE * 2, HANDLESIZE * 2));
	handles[5] = QRectF(m_CurCropRect.bottomRight()-QPointF(m_CurCropRect.width()/2, HANDLESIZE), QSizeF(HANDLESIZE * 2, HANDLESIZE * 2));
	handles[6] = QRectF(m_CurCropRect.bottomLeft()+ QPoint(-HANDLESIZE, -HANDLESIZE), QSizeF(HANDLESIZE * 2, HANDLESIZE * 2));
	handles[7] = QRectF(m_CurCropRect.bottomLeft() - QPointF(HANDLESIZE,m_CurCropRect.height() / 2), QSizeF(HANDLESIZE * 2, HANDLESIZE * 2));
}

void CropItem::keyPressEvent(QKeyEvent* evt)
{
	if (evt->key() == Qt::Key_Enter)
	{
		int mm = 0;
	}
	else if (evt->key() == Qt::Key_Escape)
	{

	}
}