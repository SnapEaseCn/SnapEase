#pragma once

#include <QWidget>
#include <QLabel>
#include "public.h"

class ZoomDesktop : public QWidget
{
	Q_OBJECT

public:
	ZoomDesktop(QWidget *parent = Q_NULLPTR);
	~ZoomDesktop();

	void SetPixmap(QPixmap pixmap) { m_Pixmap = pixmap; }

signals:
	void FinishEdit();

private:
	QRect		m_rect;
	QWidget*	m_label;
	QPixmap		m_Pixmap;
	QPixmap		m_shotimg;
	QString		m_Tip;

	void paintEvent(QPaintEvent* evt);
	void mouseMoveEvent(QMouseEvent* evt);
	void mousePressEvent(QMouseEvent* evt);
	void mouseReleaseEvent(QMouseEvent* evt);
	void closeEvent(QCloseEvent *evt);
	void keyPressEvent(QKeyEvent *event);
};
