#include "ArrowItem.h"
#include <QPainter>
#include <QDebug>

ArrowItem::ArrowItem(QGraphicsItem *parent):QGraphicsItem(parent)
{

}

QRectF ArrowItem::boundingRect() const
{
	return m_BoundingRect;
}

void ArrowItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

	if (m_EndPoint.isNull() || m_StartPoint.isNull())
		return;

	painter->setRenderHint(QPainter::Antialiasing, true);

	// double distance = sqrt(pow(m_EndPoint.x() - m_StartPoint.x(), 2) + pow(m_EndPoint.y() - m_StartPoint.y(), 2));
	// 线段的方向向量
	QPointF dirvec = m_EndPoint - m_StartPoint;

	// 线段的垂直向量
	QPointF pervec(dirvec.y()*0.8, -dirvec.x()*0.8);

	// 线段的度角向量
	QPointF dirper(dirvec.y()*0.6, -dirvec.x()*0.6);

	double lenA = 0.15;
	double lenB = 0.08;
	// 计算三角形的顶点坐标
	QPointF vertexA = m_EndPoint - lenA * dirvec*0.8 - lenB * dirper*0.8;
	QPointF vertex2 = m_EndPoint - lenA * dirvec - lenA * pervec;
	QPointF vertex1 = m_EndPoint;
	QPointF vertex3 = m_EndPoint - lenA * dirvec + lenA * pervec;
	QPointF vertexB = m_EndPoint - lenA * dirvec*0.8 + lenB * dirper*0.8;

	// 创建箭头的路径
	QPainterPath path(m_StartPoint);
	// 添加箭头的轮廓，可以根据需要自定义箭头的形状和大小
	path.lineTo(vertexA);
	path.lineTo(vertex2);
	path.lineTo(vertex1);
	path.lineTo(vertex3);
	path.lineTo(vertexB);
	path.closeSubpath();

	// 设置箭头的填充色
	painter->setBrush(m_Color);
	// 设置边框
	painter->setPen(m_Color);	
	painter->drawPath(path);
}