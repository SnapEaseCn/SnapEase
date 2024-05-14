#pragma once

#include <QDialog>
#include "ui_memdlg.h"
#include "public.h"

class MemDlg : public QDialog
{
	Q_OBJECT

public:
	MemDlg(QWidget *parent = Q_NULLPTR);
	~MemDlg();

	enum ValIndex
	{
		INDEX_OCTAL=0,		// 10进制
		INDEX_BINARY,		// 二进制
		INDEX_DECIMAL,		// 8进制
		INDEX_HEX			// 16进制
	};

private slots:
	void ConvertData();

private:
	Ui::MemDlg ui;

	void closeEvent(QCloseEvent *event);
};
