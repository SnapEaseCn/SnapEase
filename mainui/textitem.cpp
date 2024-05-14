#include "textitem.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneHoverEvent>
#include <QCursor>

TextItem::TextItem(QGraphicsItem *parent):QGraphicsItem(parent)
{
	setFlag(QGraphicsItem::ItemIsSelectable);
	//setFlag(QGraphicsItem::ItemIsMovable);
	setAcceptHoverEvents(true);
}

QRectF TextItem::boundingRect() const
{
	return m_BoundingRect;
}

void TextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

	painter->setRenderHint(QPainter::Antialiasing);

	// 设置字体和大小
	m_font.setPointSize(m_size);
	painter->setFont(m_font);
	painter->setPen(m_color);
	painter->setBrush(m_color);
	painter->drawText(m_point, m_text);
}

void TextItem::SetBoundingRect(QRectF rect) 
{ 
	m_BoundingRect = rect;
}

void TextItem::SetText(QString text, QColor clr, QFont font, int fontsize)
{
	m_text = text;
	m_font = font;
	m_size = fontsize;
	m_color = clr;
}

void TextItem::mouseMoveEvent(QMouseEvent* evt)
{
	return;
}