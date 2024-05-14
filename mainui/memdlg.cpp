#include "memdlg.h"
#include <QtConcurrent>
#include <string>
#include <cctype>
#include <bitset>
#include <algorithm>

MemDlg::MemDlg(QWidget *parent):QDialog(parent)
{
	ui.setupUi(this);
	Qt::WindowFlags flags = windowFlags();
	//flags |= Qt::WindowMinMaxButtonsHint;
	flags |= Qt::WindowCloseButtonHint;
	this->setWindowFlags(flags);

	connect(ui.pbConvert, &QPushButton::clicked, this, &MemDlg::ConvertData);
	connect(ui.pbClose, &QPushButton::clicked, this, &MemDlg::close);
}

MemDlg::~MemDlg()
{
}

inline bool isBinary(const std::string& s)
{
	for (char c : s)
	{
		if (c != '0' && c != '1') return false;
	}
	return !s.empty();
}

inline bool isOctal(const std::string& s)
{
	for (char c : s)
	{
		if (c < '0' || c > '7') return false;
	}
	return !s.empty();
}

inline bool isDecimal(const std::string& s)
{
	for (char c : s)
	{
		if (!std::isdigit(c)) return false;
	}
	return !s.empty();
}

inline bool isHexadecimal(const std::string& s) 
{
	for (char c : s)
	{
		if (!std::isxdigit(c)) return false;
	}
	return !s.empty();
}

inline std::string decimalToBase(int decimal, int base) 
{
	std::string result;
	const char digits[] = "0123456789ABCDEF"; // 用于表示数字的字符

	// 检查基数是否在允许的范围内
	if (base < 2 || base > 16)
	{
		return "Error: base out of range.";
	}

	// 处理转换
	while (decimal > 0) 
	{
		result += digits[decimal % base]; // 获取余数并转换为对应的字符
		decimal /= base; // 更新数字
	}

	// 如果结果为空，返回 "0"
	if (result.empty()) 
	{
		return "0";
	}

	// 反转字符串以获得正确的顺序
	std::reverse(result.begin(), result.end());
	return result;
}

inline std::string decimalToAnyBase(int decimal, int base) 
{
	std::string result;
	const std::string digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // 支持到 36 进制的字符

	// 检查基数是否在允许的范围内
	if (base < 2 || base > 36) 
	{
		return "Error: base out of range.";
	}

	// 特殊情况处理：当数字为 0 时
	if (decimal == 0) 
	{
		return "0";
	}

	// 处理负数
	bool isNegative = false;
	if (decimal < 0) 
	{
		isNegative = true;
		decimal = -decimal; // 取绝对值
	}

	// 转换过程
	while (decimal > 0) 
	{
		result += digits[decimal % base];
		decimal /= base;
	}

	// 添加负号（如果需要）
	if (isNegative) 
	{
		result += '-';
	}

	// 反转字符串以获得正确的顺序
	std::reverse(result.begin(), result.end());

	return result;
}

void MemDlg::ConvertData()
{
	QString input = ui.lineEditInput->text();
	if (!input.isEmpty())
	{
		int target = 0;
		std::string val = input.toStdString();
		int idx = ui.comboBox->currentIndex();
		if (idx == INDEX_BINARY && isBinary(val))
		{
			target = std::stoi(val, 0, 2);
		}
		else if (idx == INDEX_DECIMAL && isOctal(val))
		{
			target = std::stoi(val, nullptr, 8);
		}
		else if (idx == INDEX_OCTAL && isDecimal(val))
		{
			target = std::stoi(val, nullptr, 10);
		}
		else if (idx == INDEX_HEX && isHexadecimal(val))
		{
			target = std::stoi(val, nullptr, 16);
		}

		if (target != 0)
		{
			ui.lineEdit_Out2->setText(decimalToBase(target,2).c_str());
			ui.lineEdit_Out8->setText(decimalToBase(target, 8).c_str());
			ui.lineEdit_Out10->setText(QString::number(target));
			ui.lineEdit_Out16->setText(decimalToBase(target, 16).c_str());

			int basenum = ui.spinBox->value();
			ui.lineEdit_OutRes->setText(decimalToAnyBase(target, basenum).c_str());
		}
		else
		{
			ui.lineEdit_Out2->clear();
			ui.lineEdit_Out8->clear();
			ui.lineEdit_Out10->clear();
			ui.lineEdit_Out16->clear();
			ui.lineEdit_OutRes->clear();
		}
	}
}

void MemDlg::closeEvent(QCloseEvent *event)
{
	this->deleteLater();
	QDialog::closeEvent(event);
}