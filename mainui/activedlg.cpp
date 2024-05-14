#include "ActiveDlg.h"
#include "text.h"
#include <QtConcurrent>
#include <QCryptographicHash>

ActiveDlg::ActiveDlg(QWidget *parent):QDialog(parent)
{
	ui.setupUi(this);
	Qt::WindowFlags flags = windowFlags();
	//flags |= Qt::WindowMinMaxButtonsHint;
	flags |= Qt::WindowCloseButtonHint;
	this->setWindowFlags(flags);

	connect(ui.tbActive, &QPushButton::clicked, this, &ActiveDlg::ActiveSnapEase);
	connect(ui.pbClose, &QPushButton::clicked, this, &ActiveDlg::reject);
	
}

ActiveDlg::~ActiveDlg()
{
}

void ActiveDlg::SetCode(std::string code)
{
	ui.lineEditCode->setText(Str2QStr(code));
}

void ActiveDlg::SetMoneyCode(std::string moneycode)
{
	ui.labelMoneyCode->setText(Str2QStr(moneycode));
}

void ActiveDlg::ActiveSnapEase()
{
	QString serialcode = ui.lineEditCode->text();
	QString activecode = ui.lineEditActiveCode->text();

	if (!serialcode.isEmpty() && !activecode.isEmpty())
	{
		serialcode.append("leeme").append("SnapEase");
		// MD5¼ÓÃÜ
		QByteArray md5Hash = QCryptographicHash::hash(serialcode.toUtf8(), QCryptographicHash::Md5);

		// SHA-1¼ÓÃÜ
		QByteArray sha1Hash = QCryptographicHash::hash(md5Hash, QCryptographicHash::Sha1);
		QString shadata = sha1Hash.toHex().toUpper();
		if (activecode.trimmed() == shadata)
		{
			this->accept();
		}
		else
		{
			ui.labelTip->setText(tr("Activation failed, the activation code is incorrect!"));
		}
	}
}