#include "aboutus.h"
#include <QtConcurrent>

AboutUs::AboutUs(QWidget *parent):QDialog(parent)
{
	ui.setupUi(this);
	Qt::WindowFlags flags = windowFlags();
	flags |= Qt::WindowMinMaxButtonsHint;
	flags |= Qt::WindowCloseButtonHint;
	this->setWindowFlags(flags);
}

AboutUs::~AboutUs()
{
}

void AboutUs::closeEvent(QCloseEvent *event)
{
	this->deleteLater();
	QDialog::closeEvent(event);
}