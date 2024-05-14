#include "ScreenDlg.h"
#include <QtConcurrent>
#include <QPainter>
#include <QMouseEvent>
#include <QIcon>
#include "text.h"

ScreenDlg::ScreenDlg(QWidget *parent):QWidget(parent)
{
	setWindowState(Qt::WindowFullScreen); // 设置为全屏状态
	std::string path = IMAGE_ICO("logo");
	setWindowIcon(QIcon(Str2QStr(path)));

	setWindowState(Qt::WindowFullScreen); // 设置为全屏状态
	//setWindowOpacity(0.2); // 设置不透明度（范围：0 到 1）
	m_bPress = false;
	setMouseTracking(true);

	// 设置鼠标形状
	setCursor(Qt::CrossCursor);
}

ScreenDlg::~ScreenDlg()
{
}

void ScreenDlg::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	//painter.fillRect(rect(), QBrush(QColor(0, 255, 100, 100))); // 添加半透明的背景
	painter.drawPixmap(0, 0, m_Pixmap);

	if (m_bPress)
	{
		QPen pen(QColor(255, 0, 0,255), 2);
		painter.setPen(pen);
		painter.drawRect(m_rect); // 绘制选择框
	}
	else if(!m_CurPoint.isNull())
	{
		QPen pen(QColor(22, 160, 144, 255), 1);
		painter.setPen(pen);
		painter.drawLine(QPoint(m_CurPoint.x(),-60000), QPoint(m_CurPoint.x(), 60000));
		painter.drawLine(QPoint(-60000,m_CurPoint.y()), QPoint(60000,m_CurPoint.y()));
	}

	QWidget::paintEvent(event);
}

void ScreenDlg::mouseMoveEvent(QMouseEvent* evt)
{
	if (m_bPress)
	{
		m_rect.setBottomRight(evt->pos());
		this->update();
	}
	else
	{
		m_CurPoint = evt->pos();
		this->update();
	}
	QWidget::mouseMoveEvent(evt);
}

void ScreenDlg::mousePressEvent(QMouseEvent* evt)
{
	m_bPress = true;
	m_rect.setTopLeft(evt->pos());
	QWidget::mousePressEvent(evt);
}

void ScreenDlg::mouseReleaseEvent(QMouseEvent* evt)
{
	FinishSelRect(m_rect);
	m_bPress = false;
	QWidget::mousePressEvent(evt);
}

void ScreenDlg::closeEvent(QCloseEvent *evt)
{
	QWidget::closeEvent(evt);
}

void ScreenDlg::keyPressEvent(QKeyEvent *event)
{
	// 检查按下的键是否为 ESC 键
	if (event->key() == Qt::Key_Escape)
	{
		emit FinishSelRect(QRect(0, 0, 0, 0));
		event->accept();
	}
	else
	{
		// 未处理的键盘事件由基类处理函数进行处理
		QWidget::keyPressEvent(event);
	}
}