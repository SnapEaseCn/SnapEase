#pragma once

#include <QDialog>
#include "ui_ColorList.h"
#include "public.h"
#include <QFutureWatcher>
#include <QFuture>
#include <QTimer>

class QImage;
class KMeans;
class ColorList : public QDialog
{
	Q_OBJECT

public:
	ColorList(QWidget *parent = Q_NULLPTR);
	~ColorList();

private slots:
	void SellectFile();
	void GeneralColor();
	void FinishCurrentRun();
	void ColorClicked();
	void CopyColor();
	void ColorTopicChange(int);
	void ShowRemainTime();

private:
	Ui::ColorList ui;

	unsigned int	m_iRemainTime;
	bool		m_bRun;
	short		m_iAlgType;
	short		m_IterationsNum;
	QImage*		m_image;
	QTimer		m_timer;
	KMeans*		m_KMeans;
	std::map<int, QToolButton*> map_color;
	std::map<int, std::vector<unsigned int>>	 custom_color;
	std::vector<unsigned int>	vec_colors;
	QFutureWatcher<void>*		m_pFutureWatcher;
	QFuture<void>				m_future;

	void RunGeneralColor();
	void InitCustomColor();
	void UpdateRemainTime(unsigned int);
	void closeEvent(QCloseEvent *event);
};
