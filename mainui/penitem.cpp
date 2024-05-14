#include "penitem.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneHoverEvent>
#include <QCursor>

PenItem::PenItem(QGraphicsItem *parent):QGraphicsItem(parent)
{
	setFlag(QGraphicsItem::ItemIsSelectable);
	setAcceptHoverEvents(true);
}

QRectF PenItem::boundingRect() const
{
	return m_BoundingRect;
}

void PenItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

	for (const auto& itr : vec_path)
	{
		QColor clr = itr.first;
		painter->setPen(QPen(clr,3));
		const QPainterPath& path = itr.second;
		// »æÖÆÂ·¾¶£¨Ïß¶Î£©
		painter->drawPath(path);
	}
}

void PenItem::SetBoundingRect(QRectF rect) 
{ 
	m_BoundingRect = rect;
}

void PenItem::AddToPath(const QPointF& point)
{
	if (vec_path.size() > 0)
	{
		vec_path.back().second.lineTo(point);
	}
}

void PenItem::AddNewPath(QColor clr, QPointF point)
{
	vec_path.push_back(std::pair<QColor, QPainterPath>(clr, point));
}