#include "TimeDlg.h"
#include <QtConcurrent>

TimeDlg::TimeDlg(QWidget *parent):QDialog(parent)
{
	ui.setupUi(this);
	Qt::WindowFlags flags = windowFlags();
	/*flags |= Qt::WindowMinMaxButtonsHint;*/
	flags |= Qt::WindowCloseButtonHint;
	this->setWindowFlags(flags);

	ui.dateTimeTick->setDateTime(QDateTime::currentDateTime());
	ui.dateTimeBirth->setDateTime(QDateTime::currentDateTime());

	connect(ui.tbTimeToTick, &QToolButton::clicked, this, &TimeDlg::ConvertTimeToTick);
	connect(ui.tbTickToTime, &QToolButton::clicked, this, &TimeDlg::ConvertTickToTime);
	connect(ui.tbCaclOldBirth, &QToolButton::clicked, this, &TimeDlg::CaclOldBirth);
}

TimeDlg::~TimeDlg()
{
}

void TimeDlg::closeEvent(QCloseEvent *event)
{
	this->deleteLater();
	QDialog::closeEvent(event);
}

void TimeDlg::ConvertTimeToTick()
{
	QDateTime dateTime = ui.dateTimeTick->dateTime();
	unsigned int tick = dateTime.toSecsSinceEpoch();
	ui.lineEditTick->setText(QString::number(tick));
}

void TimeDlg::ConvertTickToTime()
{
	QString text = ui.lineEditDateTick->text();
	bool bOk = false;
	unsigned int tick = text.toUInt(&bOk);
	if (bOk)
	{
		QDateTime dateTime = QDateTime::fromSecsSinceEpoch(tick);
		ui.lineEditDate->setText(dateTime.toString("yyyy-MM-dd HH:mm:ss"));
	}
}

void TimeDlg::CaclOldBirth()
{
	QDateTime birthDate = ui.dateTimeBirth->dateTime();
	QDateTime currentDate = QDateTime::currentDateTime();

	// Calculate the years difference
	int age = currentDate.date().year() - birthDate.date().year();

	// Adjust the age if the current date is before the birth date in the current year
	if (currentDate.date() < birthDate.date().addYears(age))
	{
		age--;
	}
	ui.lineEditBirth->setText(QString::number(age));
}