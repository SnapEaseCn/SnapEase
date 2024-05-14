#pragma once

#include <QDialog>
#include "ui_ResizeDlg.h"
#include "public.h"

class ResizeDlg : public QDialog
{
	Q_OBJECT

public:
	ResizeDlg(QWidget *parent = Q_NULLPTR);
	~ResizeDlg();

	void SetImageSize(unsigned int width, unsigned int height);
	void GetImageSize(unsigned int& width,unsigned int& height);

private slots:
	void HeightChange();
	void WidthChange();
	void FinishDone();

signals:
	void FinishResize(int,int);

private:
	Ui::ResizeDlg ui;

	int		m_iWidth;
	int		m_iHeight;

	int		m_iLastWidth;
	int		m_iLastHeight;
};
