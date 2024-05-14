#include "TextEdit.h"
#include <QRegularExpression>
#include <QtConcurrent>
#include <QDomDocument>
#include <QApplication>
#include <QClipboard>

TextEdit::TextEdit(QWidget *parent):QDialog(parent)
{
	ui.setupUi(this);
	Qt::WindowFlags flags = windowFlags();
	flags |= Qt::WindowCloseButtonHint;
	this->setWindowFlags(flags);

	ui.plainTextEdit->setWordWrapMode(QTextOption::WrapAnywhere);
	connect(ui.pbDone, &QPushButton::clicked, this, &TextEdit::EditTextDone);
	connect(ui.pbCopy, &QPushButton::clicked, this, &TextEdit::CopyTextToClipBoard);
}

TextEdit::~TextEdit()
{
}

void TextEdit::closeEvent(QCloseEvent *event)
{
	this->deleteLater();
	QDialog::closeEvent(event);
}

void TextEdit::EditTextDone()
{
	QString text = ui.plainTextEdit->toPlainText();
	if (!text.isEmpty())
	{
		int idx = ui.comboBox->currentIndex();
		switch (idx)
		{
			case EDIT_SPACES:
			{
				// Removes all whitespace characters (space, tab, newline, etc.)
				text = text.remove(QRegularExpression("\\s+"));
				ui.plainTextEdit->setPlainText(text);
			}
			break;
			case EDIT_FORMAT_JSON:
			{
				QString out;
				if (FormatJson(text, out))
				{
					ui.plainTextEdit->setPlainText(out);
				}
				else
				{
					ui.plainTextEdit->setPlainText(out+text);
				}
			}
			break;
			case EDIT_FORMAT_XML:
			{
				QString out;
				if (FormatXml(text, out))
				{
					ui.plainTextEdit->setPlainText(out);
				}
				else
				{
					ui.plainTextEdit->setPlainText(out + text);
				}
			}
			break;
			case EDIT_FORMAT_UPPER:
			{
				ui.plainTextEdit->setPlainText(text.toUpper());
			}
			break;
			case EDIT_FORMAT_SMALL:
			{
				ui.plainTextEdit->setPlainText(text.toLower());
			}
			break;
			default:
				break;
		}
	}
}

bool TextEdit::FormatJson(const QString& jsonText, QString& output)
{
	QJsonParseError error;
	QJsonDocument doc = QJsonDocument::fromJson(jsonText.toUtf8(), &error);

	if (error.error == QJsonParseError::NoError) 
	{
		// Indented for pretty printing
		output = doc.toJson(QJsonDocument::Indented);
		return true;  
	}
	else 
	{
		// Handle the error
		output = "Invalid JSON:" + error.errorString()+"\n";
		return false;
	}
}

bool TextEdit::FormatXml(const QString& xmlText, QString& xml)
{
	QDomDocument doc;

	// Parse the XML
	if (doc.setContent(xmlText)) 
	{
		// Serialize the document to a string with indentation
		QTextStream stream(&xml);
		doc.save(stream, 4);  // '4' is the indentation level
		return true;
	}
	else 
	{
		// Handle parsing error
		xml = "Error parsing XML\n";
		return false;
	}
}

void TextEdit::CopyTextToClipBoard()
{
	QString text = ui.plainTextEdit->toPlainText();
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(text);
}