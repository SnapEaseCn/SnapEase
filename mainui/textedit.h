#pragma once

#include <QDialog>
#include "ui_textedit.h"
#include "public.h"

class TextEdit : public QDialog
{
	Q_OBJECT

public:
	TextEdit(QWidget *parent = Q_NULLPTR);
	~TextEdit();

	enum EditType
	{
		EDIT_SPACES =0,			// 清除空格
		EDIT_FORMAT_JSON,		// JSON格式化
		EDIT_FORMAT_XML,		// XML格式化
		EDIT_FORMAT_UPPER,		// 全部转大写
		EDIT_FORMAT_SMALL,		// 全部转小写

	};

private slots:
	void EditTextDone();
	void CopyTextToClipBoard();

private:
	Ui::TextEdit ui;

	void closeEvent(QCloseEvent *event);
	bool FormatJson(const QString& jsonText, QString& output);
	bool FormatXml(const QString& xmlText, QString& xml);
};
