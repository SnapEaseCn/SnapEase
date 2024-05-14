#pragma once

#include <QDialog>
#include "ui_aboutdlg.h"
#include "public.h"

class AboutDlg : public QDialog
{
	Q_OBJECT

public:
	AboutDlg(QWidget *parent = Q_NULLPTR);
	~AboutDlg();

private:
	Ui::AboutDlg ui;

	void closeEvent(QCloseEvent *event);
};
