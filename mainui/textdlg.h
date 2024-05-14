#pragma once

#include <QDialog>
#include "ui_textdlg.h"
#include "public.h"

class TextDlg : public QDialog
{
	Q_OBJECT

public:
	TextDlg(QWidget *parent = Q_NULLPTR);
	~TextDlg();

	void SetUpdateFont(std::function<void(QString text, QColor clr, QFont font, int size)> fun) { UPDATE_FONT = fun; }
	void SetExitFun(std::function<void(void)> fun) { EXIT_TEXT = fun; }
	void SetColor(QColor clr) { m_color = clr; }
private slots:
	void UpdateFont();
	void SelectColor();

private:
	Ui::TextDlg ui;

	std::function<void(QString, QColor, QFont, int)> UPDATE_FONT;
	std::function<void(void)> EXIT_TEXT;
	QColor	m_color;

	void closeEvent(QCloseEvent*);
};
