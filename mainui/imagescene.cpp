#include "imagescene.h"
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

ImageScene::ImageScene(QObject* parent):QGraphicsScene(parent)
{
	// ���캯��
}

void ImageScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	// ������갴���¼�
	QPointF pos = event->scenePos();
	qDebug() << "Mouse Press Event at scene position:" << pos;

	// Ĭ�ϵ���갴���¼�������
	QGraphicsScene::mousePressEvent(event);
}

void ImageScene::keyPressEvent(QKeyEvent* event)
{
	// ������̰����¼�
	int key = event->key();
	qDebug() << "Key Press Event - Key code:" << key;

	// Ĭ�ϵļ��̰����¼�������
	QGraphicsScene::keyPressEvent(event);
}

void ImageScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
	// �����Ҽ��˵��¼�
	QPointF pos = event->scenePos();
	qDebug() << "Context Menu Event at scene position:" << pos;

	// Ĭ�ϵ��Ҽ��˵��¼�������
	QGraphicsScene::contextMenuEvent(event);
}