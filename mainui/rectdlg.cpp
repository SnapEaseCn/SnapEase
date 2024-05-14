#include "RectDlg.h"
#include <QtConcurrent>
#include <QPainter>

RectDlg::RectDlg(QWidget *parent):QWidget(parent)
{
	Qt::WindowFlags flags = windowFlags();
	flags |= Qt::WindowMinMaxButtonsHint;
	flags |= Qt::WindowCloseButtonHint;
	this->setWindowFlags(flags);

	m_bComfirm = false;

	// 窗口背景透明
	//setAttribute(Qt::WA_TranslucentBackground); 
	setWindowOpacity(0.1);
	setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
}

RectDlg::~RectDlg()
{
}

void RectDlg::closeEvent(QCloseEvent *event)
{
	this->deleteLater();
	QWidget::closeEvent(event);
}

void RectDlg::paintEvent(QPaintEvent* evt)
{
	QPainter painter(this);
	//painter.drawPixmap(0, 0, m_Pixmap);

	// 设置画刷为透明
	painter.setBrush(Qt::NoBrush); 
	// 设置画笔颜色和宽度
	painter.setPen(QPen(Qt::red, 4));
	painter.drawRect(this->rect());
}

void RectDlg::mousePressEvent(QMouseEvent* evt)
{
	m_bComfirm = true;
	FINISH_SELECT(m_rect.x(), m_rect.y(), m_rect.width(), m_rect.height());
}

void RectDlg::mouseMoveEvent(QMouseEvent* evt)
{
	return;
}

void RectDlg::SetRect(QRect rect)
{
	if (!m_bComfirm)
	{
		this->setFixedWidth(rect.width());
		this->setFixedHeight(rect.height());
		this->move(rect.x(), rect.y());

		m_rect = rect;
	}
}