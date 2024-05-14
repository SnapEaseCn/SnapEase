#ifndef CROPITEM_H
#define CROPITEM_H

#include <QGraphicsItem>

class QGraphicsEllipseItem;
class CropItem : public QGraphicsItem
{
public:
    CropItem(QGraphicsItem *parent = nullptr);

	void SetBoundingRect(QRectF rect);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
	QRectF GetCropRect();

private:
	QRectF		m_BoundingRect;
	QRectF		m_CurCropRect;
	QPointF		m_dragStartPos;
	int			m_iSelHandleIdx;

	std::vector<QRectF> handles;

	void UpdateHandles();
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
	void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;
};

#endif