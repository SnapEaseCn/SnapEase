#pragma once

#include <QDialog>
#include "ui_aboutus.h"
#include "public.h"

class AboutUs : public QDialog
{
	Q_OBJECT

public:
	AboutUs(QWidget *parent = Q_NULLPTR);
	~AboutUs();

private:
	Ui::AboutUs ui;

	void closeEvent(QCloseEvent *event);
};
