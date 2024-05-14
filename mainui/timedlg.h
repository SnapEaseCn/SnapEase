#pragma once

#include <QDialog>
#include "ui_TimeDlg.h"
#include "public.h"

class TimeDlg : public QDialog
{
	Q_OBJECT

public:
	TimeDlg(QWidget *parent = Q_NULLPTR);
	~TimeDlg();

private slots:
	void ConvertTimeToTick();
	void ConvertTickToTime();
	void CaclOldBirth();

private:
	Ui::TimeDlg ui;

	void closeEvent(QCloseEvent *event);
};
