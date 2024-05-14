#pragma once

#include <QDialog>
#include "ui_ColorDlg.h"
#include "public.h"

class ColorDlg : public QDialog
{
	Q_OBJECT

public:
	ColorDlg(QWidget *parent = Q_NULLPTR);
	~ColorDlg();

	void SetColor(int,int,int);

signals:
	void FinishEdit(bool);

private slots:
	void CopyColor();
	void PickOtherColor();

private:
	Ui::ColorDlg ui;

	void closeEvent(QCloseEvent*);
};
