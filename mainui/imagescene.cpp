#include "imagescene.h"
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

ImageScene::ImageScene(QObject* parent):QGraphicsScene(parent)
{
	// 构造函数
}

void ImageScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	// 处理鼠标按下事件
	QPointF pos = event->scenePos();
	qDebug() << "Mouse Press Event at scene position:" << pos;

	// 默认的鼠标按下事件处理方法
	QGraphicsScene::mousePressEvent(event);
}

void ImageScene::keyPressEvent(QKeyEvent* event)
{
	// 处理键盘按下事件
	int key = event->key();
	qDebug() << "Key Press Event - Key code:" << key;

	// 默认的键盘按下事件处理方法
	QGraphicsScene::keyPressEvent(event);
}

void ImageScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
	// 处理右键菜单事件
	QPointF pos = event->scenePos();
	qDebug() << "Context Menu Event at scene position:" << pos;

	// 默认的右键菜单事件处理方法
	QGraphicsScene::contextMenuEvent(event);
}