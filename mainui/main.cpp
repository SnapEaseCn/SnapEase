#include "mainui.h"
#include <QApplication>
#include <QSystemTrayIcon>
#include <QDesktopWidget>
#include <QTranslator>
#include "activedlg.h"

#include <windows.h>
#include "DbgHelp.h"
#include "public.h"
#include "text.h"

int GenerateMiniDump(PEXCEPTION_POINTERS pExceptionPointers)
{ 
	// 定义函数指针
	typedef BOOL(WINAPI * MiniDumpWriteDumpT)
		(
			HANDLE,
			DWORD,
			HANDLE,
			MINIDUMP_TYPE,
			PMINIDUMP_EXCEPTION_INFORMATION,
			PMINIDUMP_USER_STREAM_INFORMATION,
			PMINIDUMP_CALLBACK_INFORMATION
			);
	// 从 "DbgHelp.dll" 库中获取 "MiniDumpWriteDump" 函数
	MiniDumpWriteDumpT pfnMiniDumpWriteDump = NULL;
	HMODULE hDbgHelp = LoadLibrary(L"DbgHelp.dll");
	if (NULL == hDbgHelp)
	{
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	pfnMiniDumpWriteDump = (MiniDumpWriteDumpT)GetProcAddress(hDbgHelp, "MiniDumpWriteDump");

	if (NULL == pfnMiniDumpWriteDump)
	{
		FreeLibrary(hDbgHelp);
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	// 创建 dmp 文件件
	TCHAR szFileName[MAX_PATH] = { 0 };
	SYSTEMTIME stLocalTime;
	GetLocalTime(&stLocalTime);
	wsprintf(szFileName, L"%04d%02d%02d-%02d%02d%02d.dmp",
		stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
		stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond);
	HANDLE hDumpFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	if (INVALID_HANDLE_VALUE == hDumpFile)
	{
		FreeLibrary(hDbgHelp);
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	// 写入 dmp 文件
	MINIDUMP_EXCEPTION_INFORMATION expParam;
	expParam.ThreadId = GetCurrentThreadId();
	expParam.ExceptionPointers = pExceptionPointers;
	expParam.ClientPointers = FALSE;
	pfnMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
		hDumpFile, MiniDumpWithDataSegs, (pExceptionPointers ? &expParam : NULL), NULL, NULL);
	// 释放文件
	CloseHandle(hDumpFile);
	FreeLibrary(hDbgHelp);
	return EXCEPTION_EXECUTE_HANDLER;
}

LONG WINAPI ExceptionFilter(LPEXCEPTION_POINTERS lpExceptionInfo)
{  
	// 这里做一些异常的过滤或提示
	if (IsDebuggerPresent())
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}
	return GenerateMiniDump(lpExceptionInfo);
}

int main(int argc, char *argv[])
{
	// 加入崩溃dump文件功能
	SetUnhandledExceptionFilter(ExceptionFilter);
	QApplication app(argc, argv);

	QLocale locale;
	QString language = locale.languageToString(locale.language());
	
	// 加载翻译文件
	QTranslator translator;
	if (language != "ChineseSS")
	{
		translator.load(":/ico/translations/qt_en.qm");
		app.installTranslator(&translator);
		translator.load(":/ico/translations/app_en.qm");
		app.installTranslator(&translator);
	}

	MainUI wnd;

	/*std::string code,moneycode;
	if (wnd.InitUserInfo(code, moneycode))
	{
		bool ret = wnd.GetUserActive();
		if (!ret)
		{
			ActiveDlg dlg;
			dlg.SetCode(code);
			dlg.SetMoneyCode(moneycode);
			if (dlg.exec() == QDialog::Rejected)
			{
				return 1;
			}
			else
			{
				// 保存激活状态
				wnd.SetActiveStatus(true);
			}
		}
	}*/

	// 创建系统托盘对象
	std::string path = IMAGE_ICO("logo");
	QSystemTrayIcon ico(QIcon(Str2QStr(path)), &wnd);
	wnd.SetTrayIcon(&ico);
	QObject::connect(&ico, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), &wnd, SLOT(TrayIconActivated(QSystemTrayIcon::ActivationReason)));

	// 获取桌面的大小
	QDesktopWidget *desktop = QApplication::desktop();
	int width = desktop->width();
	int height = desktop->height();

	// 将QWidget移动到桌面中间位置
	wnd.move((width - wnd.width())/2,10);

	wnd.show();
	ico.show();

	wnd.UpdateUserInfo();

	return app.exec();
}