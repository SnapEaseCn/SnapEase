#include "ScreenDlg.h"
#include <QtConcurrent>
#include <QPainter>
#include <QMouseEvent>
#include <QIcon>
#include "text.h"

ScreenDlg::ScreenDlg(QWidget *parent):QWidget(parent)
{
	setWindowState(Qt::WindowFullScreen); // ����Ϊȫ��״̬
	std::string path = IMAGE_ICO("logo");
	setWindowIcon(QIcon(Str2QStr(path)));

	setWindowState(Qt::WindowFullScreen); // ����Ϊȫ��״̬
	//setWindowOpacity(0.2); // ���ò�͸���ȣ���Χ��0 �� 1��
	m_bPress = false;
	setMouseTracking(true);

	// ���������״
	setCursor(Qt::CrossCursor);
}

ScreenDlg::~ScreenDlg()
{
}

void ScreenDlg::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	//painter.fillRect(rect(), QBrush(QColor(0, 255, 100, 100))); // ��Ӱ�͸���ı���
	painter.drawPixmap(0, 0, m_Pixmap);

	if (m_bPress)
	{
		QPen pen(QColor(255, 0, 0,255), 2);
		painter.setPen(pen);
		painter.drawRect(m_rect); // ����ѡ���
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
	// ��鰴�µļ��Ƿ�Ϊ ESC ��
	if (event->key() == Qt::Key_Escape)
	{
		emit FinishSelRect(QRect(0, 0, 0, 0));
		event->accept();
	}
	else
	{
		// δ����ļ����¼��ɻ��ദ�������д���
		QWidget::keyPressEvent(event);
	}
}