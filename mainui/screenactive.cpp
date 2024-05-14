#include "ScreenActive.h"
#include <QtConcurrent>
#include <QPainter>
#include <QMouseEvent>
#include <QIcon>
#include "text.h"

ScreenActive::ScreenActive(QWidget *parent):QWidget(parent)
{
	std::string path = IMAGE_ICO("logo");
	setWindowIcon(QIcon(Str2QStr(path)));

	setWindowState(Qt::WindowFullScreen); // ����Ϊȫ��״̬
	setWindowOpacity(0.2); // ���ò�͸���ȣ���Χ��0 �� 1��
}

ScreenActive::~ScreenActive()
{
}

void ScreenActive::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.fillRect(rect(), QBrush(QColor(0, 255, 100, 100))); // ��Ӱ�͸���ı���

	for (const auto& itr : map_rect)
	{
		QPen pen(QColor(255, 0, 0), 2);
		painter.setPen(pen);
		painter.drawRect(itr.second); // ����ѡ���
	}

	if (!m_rect.isEmpty())
	{
		QPen pen(QColor(255, 0, 0), 2);
		painter.setPen(pen);
		painter.drawRect(m_rect); // ����ѡ���
	}

	QWidget::paintEvent(event);
}

void ScreenActive::mouseMoveEvent(QMouseEvent* evt)
{
	for (const auto& itr : map_rect)
	{
		if (itr.second.contains(evt->pos()))
		{
			m_rect = itr.second;
			this->update();
		}
	}

	QWidget::mouseMoveEvent(evt);
}

void ScreenActive::mousePressEvent(QMouseEvent* evt)
{
	QWidget::mousePressEvent(evt);
}

void ScreenActive::mouseReleaseEvent(QMouseEvent* evt)
{
	QWidget::mousePressEvent(evt);
}

void ScreenActive::closeEvent(QCloseEvent *evt)
{
	QWidget::closeEvent(evt);
}

void ScreenActive::SetMapRect(std::map<unsigned int, QRect> rect)
{
	map_rect = rect;
}

void ScreenActive::keyPressEvent(QKeyEvent *event)
{
	// ��鰴�µļ��Ƿ�Ϊ ESC ��
	if (event->key() == Qt::Key_Escape)
	{
		emit FinishSelRect(QRect(0,0,0,0));
		event->accept();
	}
	else
	{
		// δ����ļ����¼��ɻ��ദ�������д���
		QWidget::keyPressEvent(event);
	}
}