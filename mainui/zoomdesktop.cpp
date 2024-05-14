#include "zoomdesktop.h"
#include <QtConcurrent>
#include <QPainter>
#include <QMouseEvent>
#include <QRgb>
#include <QImage>
#include <QIcon>
#include "text.h"

#define		IMAGE_WIDTH	120
#define		IMAGE_SHOT_WIDTH	30
ZoomDesktop::ZoomDesktop(QWidget *parent):QWidget(parent)
{
	setWindowState(Qt::WindowFullScreen); // 设置为全屏状态
	setMouseTracking(true);

	std::string path = IMAGE_ICO("logo");
	setWindowIcon(QIcon(Str2QStr(path)));

	// 设置鼠标形状
	setCursor(Qt::BlankCursor);
}

ZoomDesktop::~ZoomDesktop()
{
}

void ZoomDesktop::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.drawPixmap(0,0,m_Pixmap);
	painter.drawPixmap(m_rect.x(), m_rect.y(), m_shotimg);

	// 设置画笔颜色和宽度
	painter.setPen(QPen(Qt::red, 2));
	painter.drawRect(m_rect);
	painter.setPen(QPen(Qt::yellow, 2));
	painter.drawText(m_rect.bottomLeft()+QPoint(0,10), "press esc to exit");

	QWidget::paintEvent(event);
}

void ZoomDesktop::mouseMoveEvent(QMouseEvent* evt)
{		
	QPoint point = evt->pos();
	QRect captureRect(point-QPoint(IMAGE_SHOT_WIDTH, IMAGE_SHOT_WIDTH), point+QPoint(IMAGE_SHOT_WIDTH, IMAGE_SHOT_WIDTH));
	
	// 在指定矩形框区域进行裁剪
	float scale = (float)IMAGE_WIDTH / IMAGE_SHOT_WIDTH;
	m_shotimg = m_Pixmap.copy(captureRect).scaled(QSize(IMAGE_SHOT_WIDTH*2, IMAGE_SHOT_WIDTH*2)*scale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	int size = m_shotimg.size().height();
	m_rect = QRect(point-QPoint(size/2, size/2), point + QPoint(size / 2, size / 2));
	this->update();
	QWidget::mouseMoveEvent(evt);
}

void ZoomDesktop::mousePressEvent(QMouseEvent* evt)
{
	QPoint point = evt->pos();
	if (evt->button() == Qt::RightButton)
	{
		emit FinishEdit();
	}
	QWidget::mousePressEvent(evt);
}

void ZoomDesktop::keyPressEvent(QKeyEvent *event)
{
	// 检查按下的键是否为 ESC 键
	if (event->key() == Qt::Key_Escape)
	{
		emit FinishEdit();
		event->accept();
	}
	else
	{
		// 未处理的键盘事件由基类处理函数进行处理
		QWidget::keyPressEvent(event);
	}
}

void ZoomDesktop::mouseReleaseEvent(QMouseEvent* evt)
{
	QWidget::mousePressEvent(evt);
}

void ZoomDesktop::closeEvent(QCloseEvent *evt)
{
	QWidget::closeEvent(evt);
}