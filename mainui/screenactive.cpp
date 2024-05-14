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

	setWindowState(Qt::WindowFullScreen); // 设置为全屏状态
	setWindowOpacity(0.2); // 设置不透明度（范围：0 到 1）
}

ScreenActive::~ScreenActive()
{
}

void ScreenActive::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.fillRect(rect(), QBrush(QColor(0, 255, 100, 100))); // 添加半透明的背景

	for (const auto& itr : map_rect)
	{
		QPen pen(QColor(255, 0, 0), 2);
		painter.setPen(pen);
		painter.drawRect(itr.second); // 绘制选择框
	}

	if (!m_rect.isEmpty())
	{
		QPen pen(QColor(255, 0, 0), 2);
		painter.setPen(pen);
		painter.drawRect(m_rect); // 绘制选择框
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
	// 检查按下的键是否为 ESC 键
	if (event->key() == Qt::Key_Escape)
	{
		emit FinishSelRect(QRect(0,0,0,0));
		event->accept();
	}
	else
	{
		// 未处理的键盘事件由基类处理函数进行处理
		QWidget::keyPressEvent(event);
	}
}