#include "colorpicker.h"
#include <QtConcurrent>
#include <QPainter>
#include <QMouseEvent>
#include <QRgb>
#include <QImage>
#include <QIcon>
#include "text.h"

#define		IMAGE_WIDTH	120
#define		IMAGE_SHOT_WIDTH	10
ColorPicker::ColorPicker(QWidget *parent):QWidget(parent)
{
	// 设置为全屏状态
	setWindowState(Qt::WindowFullScreen);
	setMouseTracking(true);

	std::string path = IMAGE_ICO("logo");
	setWindowIcon(QIcon(Str2QStr(path)));

	// 设置鼠标形状
	path = IMAGE_ICO("xiguan");
	QPixmap ico(Str2QStr(path));
	QCursor mCur(ico,0, ico.height());
	setCursor(mCur);
}

ColorPicker::~ColorPicker()
{
}

void ColorPicker::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.drawPixmap(0,0,m_Pixmap);
	painter.drawPixmap(m_rect.x(), m_rect.y(), m_shotimg);

	// 设置画笔颜色和宽度
	painter.setPen(QPen(Qt::red, 2));
	painter.drawRect(m_rect);

	// 绘制矩形中心的十字交叉线
	painter.drawLine(m_rect.center().x(), m_rect.top(), m_rect.center().x(), m_rect.bottom());
	painter.drawLine(m_rect.left(), m_rect.center().y(), m_rect.right(), m_rect.center().y());

	if (!m_Tip.isEmpty())
	{
		painter.setPen(QPen(QColor(237, 239, 14), 2));
		painter.drawText(QPoint(m_rect.x()+10, m_rect.bottom()-10), m_Tip);
	}

	QWidget::paintEvent(event);
}

void ColorPicker::mouseMoveEvent(QMouseEvent* evt)
{		
	QPoint point = evt->pos();

	QRect captureRect(point-QPoint(IMAGE_SHOT_WIDTH, IMAGE_SHOT_WIDTH), point+QPoint(IMAGE_SHOT_WIDTH, IMAGE_SHOT_WIDTH));
	
	// 在指定矩形框区域进行裁剪
	float scale = (float)IMAGE_WIDTH / IMAGE_SHOT_WIDTH;
	m_shotimg = m_Pixmap.copy(captureRect).scaled(QSize(IMAGE_SHOT_WIDTH*2, IMAGE_SHOT_WIDTH*2)*scale);
	int size = m_shotimg.size().height();

	m_rect = QRect(m_Pixmap.width()-size, m_Pixmap.height()-size, size, size);
	if (m_rect.contains(point))
	{
		m_rect = QRect(0, 0, size, size);
	}

	QRgb color = m_Pixmap.toImage().pixel(point.x(), point.y());
	m_Tip = QString(Str2QStr("RGB(%0,%1,%2)")).arg(qRed(color)).arg(qGreen(color)).arg(qBlue(color));

	this->update();
	QWidget::mouseMoveEvent(evt);
}

void ColorPicker::mousePressEvent(QMouseEvent* evt)
{
	QPoint point = evt->pos();
	if (evt->button() == Qt::LeftButton)
	{
		QRgb color = m_Pixmap.toImage().pixel(point.x(), point.y());
		emit FinishEdit(true, qRed(color), qGreen(color), qBlue(color));
	}
	else
	{
		emit FinishEdit(false, 0, 0, 0);
	}

	QWidget::mousePressEvent(evt);
}

void ColorPicker::mouseReleaseEvent(QMouseEvent* evt)
{
	QWidget::mousePressEvent(evt);
}

void ColorPicker::closeEvent(QCloseEvent *evt)
{
	QWidget::closeEvent(evt);
}

void ColorPicker::keyPressEvent(QKeyEvent *event)
{
	// 检查按下的键是否为 ESC 键
	if (event->key() == Qt::Key_Escape)
	{
		emit FinishEdit(false, 0, 0, 0);
		event->accept();
	}
	else
	{
		// 未处理的键盘事件由基类处理函数进行处理
		QWidget::keyPressEvent(event);
	}
}