#ifndef TEXTITEM_H
#define TEXTITEM_H

#include <QGraphicsItem>
#include <QFont>
#include <QMouseEvent>

class QGraphicsEllipseItem;
class TextItem : public QGraphicsItem
{
public:
    TextItem(QGraphicsItem *parent = nullptr);

	void SetBoundingRect(QRectF rect);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
	void SetPosition(QPointF point) { m_point = point; }
	void SetText(QString text, QColor clr, QFont font, int fontsize);

private:
	QRectF		m_BoundingRect;
	QPointF		m_point;

	QString		m_text;
	QFont		m_font;
	int			m_size;
	QColor		m_color;

	void mouseMoveEvent(QMouseEvent* evt);
};

#endif