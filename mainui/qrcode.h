#pragma once

#include <QDialog>
#include <QImage>
#include "ui_qrcode.h"
#include "public.h"

class QrCode : public QDialog
{
	Q_OBJECT

public:
	QrCode(QWidget *parent = Q_NULLPTR);
	~QrCode();

private slots:
	void GeneralQrCode();
	void ColorClicked();
	void SelectColor();
	void SaveImage();

private:
	Ui::QrCode ui;

	void*	codeptr;
	std::vector<QColor>	colors;
	QColor	m_CurClr;

	void ShowCodeImage();
	void closeEvent(QCloseEvent *event);
	QImage addBorderToImage(const QImage& original, int width);
};
