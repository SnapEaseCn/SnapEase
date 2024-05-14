#pragma once

#include <QWidget>
#include "public.h"

class ScreenActive : public QWidget
{
	Q_OBJECT

public:
	ScreenActive(QWidget *parent = Q_NULLPTR);
	~ScreenActive();

	void SetMapRect(std::map<unsigned int, QRect> rect);

signals:
	void FinishSelRect(QRect);

private:
	QRect		m_rect;

	std::map<unsigned int, QRect> map_rect;

	void paintEvent(QPaintEvent* evt);
	void mouseMoveEvent(QMouseEvent* evt);
	void mousePressEvent(QMouseEvent* evt);
	void mouseReleaseEvent(QMouseEvent* evt);
	void closeEvent(QCloseEvent *evt);
	void keyPressEvent(QKeyEvent *event);
};
