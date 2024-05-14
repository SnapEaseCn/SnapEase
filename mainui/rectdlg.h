#pragma once

#include <QWidget>
#include "public.h"

class RectDlg : public QWidget
{
	Q_OBJECT

public:
	RectDlg(QWidget *parent = Q_NULLPTR);
	~RectDlg();

	void SetRect(QRect rect);
	void SetPixmap(QPixmap shot) { m_Pixmap = shot; }

	void SetFunCall(std::function<void(int, int, int, int)> fun) { FINISH_SELECT = fun; }

private:
	void closeEvent(QCloseEvent *event);
	void paintEvent(QPaintEvent* evt);
	void mousePressEvent(QMouseEvent* evt);
	void mouseMoveEvent(QMouseEvent* evt);

	bool		m_bComfirm;
	QPixmap		m_Pixmap;
	QRect		m_rect;

	std::function<void(int x, int y, int width, int height)> FINISH_SELECT;
};
