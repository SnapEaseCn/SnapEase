#pragma once

#include <QDialog>
#include "ui_activedlg.h"
#include "public.h"

class ActiveDlg : public QDialog
{
	Q_OBJECT

public:
	ActiveDlg(QWidget *parent = Q_NULLPTR);
	~ActiveDlg();

	void SetCode(std::string code);
	void SetMoneyCode(std::string moneycode);

private slots:
	void ActiveSnapEase();

private:
	Ui::ActiveDlg ui;
};
