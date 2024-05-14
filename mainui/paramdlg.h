#pragma once

#include <QDialog>
#include "ui_paramdlg.h"
#include "public.h"

class paramdlg : public QDialog
{
	Q_OBJECT

public:
	paramdlg(QWidget *parent = Q_NULLPTR);
	~paramdlg();

private:
	Ui::paramdlg ui;

	void closeEvent(QCloseEvent *event);
};
