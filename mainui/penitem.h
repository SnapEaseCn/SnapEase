#ifndef PENITEM_H
#define PENITEM_H

#include <QGraphicsItem>

class QGraphicsEllipseItem;
class PenItem : public QGraphicsItem
{
public:
    PenItem(QGraphicsItem *parent = nullptr);

	void SetBoundingRect(QRectF rect);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
	void AddToPath(const QPointF& point);
	void AddNewPath(QColor clr, QPointF point);

private:
	QRectF		m_BoundingRect;
	QPointF		m_dragStartPos;

	std::vector<std::pair<QColor, QPainterPath>> vec_path;
};

#endif