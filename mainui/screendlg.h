#pragma once

#include <QWidget>
#include "public.h"

class ScreenDlg : public QWidget
{
	Q_OBJECT

public:
	ScreenDlg(QWidget *parent = Q_NULLPTR);
	~ScreenDlg();

	void SetPixMap(QPixmap shot) { m_Pixmap = shot; }

signals:
	void FinishSelRect(QRect);

private:
	bool		m_bPress;
	QRect		m_rect;
	QPixmap		m_Pixmap;
	QPoint		m_CurPoint;

	void paintEvent(QPaintEvent* evt);
	void mouseMoveEvent(QMouseEvent* evt);
	void mousePressEvent(QMouseEvent* evt);
	void mouseReleaseEvent(QMouseEvent* evt);
	void closeEvent(QCloseEvent *evt);
	void keyPressEvent(QKeyEvent *event);
};
