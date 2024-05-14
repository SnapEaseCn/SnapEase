#ifndef ARROWITEM_H
#define ARROWITEM_H

#include <QGraphicsItem>
class ArrowItem : public QGraphicsItem
{
public:
    ArrowItem(QGraphicsItem *parent = nullptr);

	void SetBoundingRect(QRectF rect) { m_BoundingRect = rect; }
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

	void SetStartPoint(QPointF point) { m_StartPoint = point; }
	void SetEndPoint(QPointF point) { m_EndPoint = point; }
	void SetColor(QColor clr) { m_Color = clr; }

private:
	QPointF		m_StartPoint;
	QPointF		m_EndPoint;
	QRectF		m_BoundingRect;
	QColor		m_Color;
};

#endif