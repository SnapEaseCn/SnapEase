#include "mainui.h"
#include "text.h"
#include "socket.h"
#include <QFileDialog>
#include <QTimer>
#include <functional>
#include <algorithm>
#include <cctype>
#include <QImageWriter>
#include <QMessageBox>
#include <QCloseEvent>
#include <QButtonGroup>
#include <limits>
#include <functional> 
#include <windowsx.h>
#include <QtConcurrent>
#include <QWindow>
#include <QScreen>
#include <QMenu>
#include <filesystem>

#include "logtxt.h"
#include "public.h"
#include "glob.h"
#include "imageedit.h"
#include "voiceedit.h"
#include "screendlg.h"
#include "colorpicker.h"
#include "colordlg.h"
#include "screenactive.h"
#include "zoomdesktop.h"
#include "recorddlg.h"
#include "memdlg.h"
#include "colorlist.h"
#include "qrcode.h"
#include "textedit.h"
#include "aboutus.h"
#include "timedlg.h"
#include "rectdlg.h"
#include "paramdlg.h"

#include <QDebug>
using namespace std::placeholders;
#define MAX_LOG_TEXT 8000

#define SERVER_IP0	"openvt.cn"
#define SERVER_PORT	8904

/**
* @brief 简单概述
* @brief 信号连接
*/
void MainUI::ConnectSignal()
{
	LOGINS->SetFolder(QStr2Str(QCoreApplication::applicationDirPath() + Str2QStr("/LOG_FILE")));

	connect(ui.tbClose, SIGNAL(clicked()), this, SLOT(CloseUI()));
	connect(ui.tbMin, SIGNAL(clicked()), this, SLOT(MinBtnClick()));

	connect(ui.tbOpen, SIGNAL(clicked()), this, SLOT(OpenImage()));
	connect(ui.tbCaptureRegion, SIGNAL(clicked()), this, SLOT(CaptureRegion()));
	connect(ui.tbCaptureFullScreen, SIGNAL(clicked()), this, SLOT(CaptureFullScreen()));
	connect(ui.tbTextVoice, SIGNAL(clicked()), this, SLOT(Text2Voice()));

	connect(ui.tbPickColor, SIGNAL(clicked()), this, SLOT(PickScreenColor()));
	connect(ui.tbZoom, SIGNAL(clicked()), this, SLOT(ZoomDeskTop()));
	connect(ui.tbColor, SIGNAL(clicked()), this, SLOT(ShowColorTable()));

	// 录屏
	//connect(ui.tbScreenRecord, SIGNAL(clicked()), this, SLOT(ScreenRecord()));

	// 滚动截屏
	//connect(ui.tbCaptureScroll, SIGNAL(clicked()), this, SLOT(CaptureScrollArea()));
}

MainUI::MainUI(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);

	ConnectSignal();
	GLOB_INS->SetAppPath(QStr2Str(QCoreApplication::applicationDirPath()));
	GLOB_INS->InitData();

	m_pImageEdit = NULL;
	m_pVoiceEdit = NULL;
	m_pScreenDlg = NULL;
	m_pColorPicker = NULL;
	m_pColorDlg = NULL;
	m_pScreenActive = NULL;
	m_pZoomDesktop = NULL;
	m_TrayIcon = NULL;
	m_OperMouse = OPER_UNKNOW;

	m_timer = new QTimer(this);
	connect(m_timer, &QTimer::timeout, this, &MainUI::OnFrameUpdate);
	m_timer->start(1000);  // 定时器设置为每秒触发一次

	// 安装全局事件过滤器
	qApp->installEventFilter(this);

	std::string path = IMAGE_ICO("cursor_move");
	QCursor mCur(QPixmap(Str2QStr(path)));
	ui.labelTitle->setCursor(mCur);
	ui.light->setCursor(mCur);

	m_MenuMore = new QMenu(this);
	path = IMAGE_ICO("jinzhi");
	QAction* actionTv = new QAction(QIcon(Str2QStr(path)), tr("计算机进制转换"), m_MenuMore);
	connect(actionTv, SIGNAL(triggered()), this, SLOT(BaseConvert()));
	m_MenuMore->addAction(actionTv);

	path = IMAGE_ICO("qrcode");
	QAction* actionCon = new QAction(QIcon(Str2QStr(path)), tr("生成二维码"), m_MenuMore);
	connect(actionCon, SIGNAL(triggered()), this, SLOT(GenaralQrCode()));
	m_MenuMore->addAction(actionCon);

	path = IMAGE_ICO("texttrim");
	QAction* actionTrim = new QAction(QIcon(Str2QStr(path)), tr("文本编辑助手"), m_MenuMore);
	connect(actionTrim, SIGNAL(triggered()), this, SLOT(DoneTextEdit()));
	m_MenuMore->addAction(actionTrim);

	path = IMAGE_ICO("time");
	QAction* actionTime = new QAction(QIcon(Str2QStr(path)), tr("时间管理助手"), m_MenuMore);
	connect(actionTime, SIGNAL(triggered()), this, SLOT(TimeConvert()));
	m_MenuMore->addAction(actionTime);

	/*path = IMAGE_ICO("time");
	QAction* actionScroll = new QAction(QIcon(Str2QStr(path)), tr("滚动截屏"), m_MenuMore);
	connect(actionScroll, SIGNAL(triggered()), this, SLOT(CaptureScrollArea()));
	m_MenuMore->addAction(actionScroll);*/

	/*path = IMAGE_ICO("Preferences");
	QAction* actionSetting = new QAction(QIcon(Str2QStr(path)), tr("参数设置"), m_MenuMore);
	connect(actionSetting, SIGNAL(triggered()), this, SLOT(ParamSetting()));
	m_MenuMore->addAction(actionSetting);*/

	path = IMAGE_ICO("download");
	QAction* actionUpdate = new QAction(QIcon(Str2QStr(path)), tr("关于我们"), m_MenuMore);
	connect(actionUpdate, SIGNAL(triggered()), this, SLOT(ShowAboutUs()));
	m_MenuMore->addAction(actionUpdate);

	ui.tbMoreOperation->setMenu(m_MenuMore);
	ui.tbMoreOperation->setPopupMode(QToolButton::InstantPopup);
	ui.tbMoreOperation->setToolButtonStyle(Qt::ToolButtonIconOnly);
	ui.tbMoreOperation->setArrowType(Qt::NoArrow);
}

MainUI::~MainUI()
{
	m_bUpdate = true;
}

void MainUI::closeEvent(QCloseEvent *evt)
{
	evt->ignore();
}

void MainUI::mouseMoveEvent(QMouseEvent * event)
{
	QPoint mousePoint = event->globalPos();
	if (!mouseQPoint.isNull())
	{
		QPoint point = mousePoint - mouseQPoint;
		this->move(point);
	}

	QWidget::mouseMoveEvent(event);
}

void MainUI::mousePressEvent(QMouseEvent * event)
{
	QPoint mousePoint = event->globalPos();
	QPoint topPoint = this->geometry().topLeft();
	if (mousePoint.x() > topPoint.x() && mousePoint.y() > topPoint.y() &&
		mousePoint.x() < (topPoint.x() + ui.labelTitle->width()+ ui.light->width()) && mousePoint.y() < (topPoint.y() + ui.labelTitle->height()+ui.light->height()))
	{
		mouseQPoint = mousePoint - topPoint;
	}

	QWidget::mousePressEvent(event);
}

void MainUI::mouseReleaseEvent(QMouseEvent * event)
{
	this->mouseQPoint = QPoint();
	QWidget::mouseReleaseEvent(event);
}

/**
* @brief 简单概述
* @brief 选取打开文件路径，精确到文件
*/
QString MainUI::SelectFileNamePath()
{
	QString fileName;
	QFileDialog file_dlg;
	file_dlg.setWindowTitle(tr("Select file"));
	file_dlg.setAcceptMode(QFileDialog::AcceptOpen);
	file_dlg.setFileMode(QFileDialog::AnyFile);
	file_dlg.setViewMode(QFileDialog::Detail);

	QStringList filters;
	filters << tr("PNG Format(*.png)") << tr("JPG Format(*.jpg;*.jpeg)") << tr("BMP Format(*.bmp)") << tr("TIF Format(*.tif;*.tiff)") << tr("All Files(*)");
	file_dlg.setNameFilters(filters);

	file_dlg.setGeometry(10, 30, 300, 200);
	file_dlg.setDirectory(Str2QStr("."));
	if (file_dlg.exec() == QDialog::Accepted)
	{
		fileName = file_dlg.selectedFiles()[0];
	}
	return fileName;
}

/**
* @brief 简单概述
* @brief 选取保存文件路径，精确到文件
*/
QString MainUI::SelectSavePath()
{
	QString fileName;
	QFileDialog file_dlg;
	file_dlg.setWindowTitle(tr("Select save path"));
	file_dlg.setAcceptMode(QFileDialog::AcceptSave);
	file_dlg.setFileMode(QFileDialog::AnyFile);
	QStringList filters;
	filters << tr("(*.mt)");
	file_dlg.setNameFilters(filters);
	file_dlg.setViewMode(QFileDialog::Detail);

	file_dlg.setGeometry(10, 30, 300, 200);
	file_dlg.setDirectory(Str2QStr("."));
	if (file_dlg.exec() == QDialog::Accepted)
	{
		fileName = file_dlg.selectedFiles()[0];
	}
	return fileName;
}

void MainUI::OpenImage()
{
	QString filePath = SelectFileNamePath();
	if (!filePath.isEmpty())
	{
		if (m_pImageEdit == NULL)
		{
			m_pImageEdit = new ImageEdit();
			connect(m_pImageEdit, SIGNAL(FinishEdit()), this, SLOT(FinishImageEdit()));
		}
		m_pImageEdit->SetFilePath(filePath);
		m_pImageEdit->show();
	}
}

void MainUI::TrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::Trigger)
	{
		if (!this->isVisible())
			this->setVisible(true);
		else
			this->setVisible(false);
	}
}

void MainUI::MinBtnClick()
{
	this->setVisible(false);
}

/**
* @brief 简单概述
* @brief 截取激活窗口
*/
void MainUI::CaptureTargetWindow()
{
	m_OperMouse = OPER_GET_ACTIVE_UI;
}

/**
* @brief 简单概述
* @brief 文本转语音.
*/
void MainUI::Text2Voice()
{
	if (m_pVoiceEdit == NULL)
	{
		m_pVoiceEdit = new VoiceEdit(this);
	}
	connect(m_pVoiceEdit, SIGNAL(Finish()), this, SLOT(VoiceEditFinish()));
	m_pVoiceEdit->show();
}

void MainUI::VoiceEditFinish()
{
	if (m_pVoiceEdit != NULL)
	{
		m_pVoiceEdit->deleteLater();
		m_pVoiceEdit = NULL;
	}
}

void MainUI::CloseUI()
{
	if (m_TrayIcon != NULL)
	{
		m_TrayIcon->hide();
	}
	QApplication::exit(0);
}

void MainUI::keyPressEvent(QKeyEvent *event)
{
	// 检查按下的键是否为 ESC 键
	if (event->key() == Qt::Key_Escape)
	{
		m_OperMouse = OPER_UNKNOW;
		event->accept();
	}
	else
	{
		// 未处理的键盘事件由基类处理函数进行处理
		QWidget::keyPressEvent(event);
	}
}

bool MainUI::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::MouseMove)
	{
		QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
		QPoint mousePos = mouseEvent->globalPos();
	}
	else if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent *evt = static_cast<QKeyEvent *>(event);
		int key = evt->key();
		if (m_pImageEdit != NULL)
		{
			m_pImageEdit->keyPressEvent(evt);
		}
	}

	return QWidget::eventFilter(obj, event);
}

void MainUI::OnFrameUpdate()
{
	
}

void MainUI::paintEvent(QPaintEvent *event)
{
	QWidget::paintEvent(event);
}

void MainUI::CaptureRegion()
{
	this->setWindowOpacity(0.0); // 设置不透明度(范围：0 到1)
	// 创建一个屏幕对象
	QScreen *screen = QGuiApplication::primaryScreen();
	// 截图
	QPixmap screenshot = screen->grabWindow(0);
	if (m_pScreenDlg == NULL)
	{
		this->setWindowOpacity(0.0); // 设置不透明度(范围：0 到1)
		m_pScreenDlg = new ScreenDlg();
		connect(m_pScreenDlg, SIGNAL(FinishSelRect(QRect)), this, SLOT(FinishSelRect(QRect)));
	}
	m_pScreenDlg->SetPixMap(screenshot);
	m_pScreenDlg->show();
}

void MainUI::FinishSelRect(QRect rect)
{
	if (m_pScreenDlg != NULL)
	{
		m_pScreenDlg->hide();
		m_pScreenDlg->deleteLater();
		m_pScreenDlg = NULL;
	}
	if (rect.width() > 0 && rect.height() > 0)
	{
		// 创建一个屏幕对象
		QScreen *screen = QGuiApplication::primaryScreen();
		// 截图
		QPixmap screenshot = screen->grabWindow(0, rect.x(), rect.y(), rect.width(), rect.height());
		if (m_pImageEdit == NULL)
		{
			m_pImageEdit = new ImageEdit();
			connect(m_pImageEdit, SIGNAL(FinishEdit()), this, SLOT(FinishImageEdit()));
		}
		m_pImageEdit->SetPixmap(screenshot);
		m_pImageEdit->show();
		m_pImageEdit->activateWindow();
		m_pImageEdit->setFocus();
		this->setWindowOpacity(1); // 设置不透明度(范围：0 到1)
	}
}

/**
* @brief 简单概述
* @brief 编辑图像完成.
*/
void MainUI::FinishImageEdit()
{
	if (m_pImageEdit != NULL)
	{
		m_pImageEdit->deleteLater();
		m_pImageEdit = NULL;
	}
}

/**
* @brief 简单概述
* @brief 截取全屏
*/
void MainUI::CaptureFullScreen()
{
	this->setWindowOpacity(0.0); // 设置不透明度(范围：0 到1)
	// 创建一个屏幕对象
	QScreen *screen = QGuiApplication::primaryScreen();
	// 截图
	QPixmap screenshot = screen->grabWindow(0);
	if (m_pImageEdit == NULL)
	{
		m_pImageEdit = new ImageEdit();
		connect(m_pImageEdit, SIGNAL(FinishEdit()), this, SLOT(FinishImageEdit()));
	}
	m_pImageEdit->SetPixmap(screenshot);
	m_pImageEdit->show();
	m_pImageEdit->activateWindow();
	m_pImageEdit->setFocus();
	this->setWindowOpacity(1); // 设置不透明度(范围：0 到1)
}

bool		bFinish = false;
RECT		m_rect;
RectDlg*	m_RectWidget=NULL;

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION) 
	{
		MSLLHOOKSTRUCT *pMouseStruct = (MSLLHOOKSTRUCT *)lParam;
		if (pMouseStruct != NULL) 
		{
			POINT pt;
			GetCursorPos(&pt);

			// 获取鼠标位置下的窗口句柄
			HWND hwnd = WindowFromPoint(pt);

			// 获取窗口的矩形区域
			RECT rect;
			GetWindowRect(hwnd, &rect);

			// 现在你可以使用 rect 的坐标
			int width = rect.right - rect.left;
			int height = rect.bottom - rect.top;

			qDebug() << "width:" << width << " height:" << height<<"\n";
			if (m_rect.left != rect.left || m_rect.right != rect.right || m_rect.top != rect.top || m_rect.bottom != rect.bottom)
			{
				m_rect = rect;
				if (!bFinish && m_RectWidget == NULL)
				{
					m_RectWidget = new RectDlg();
					QRect rect(QPoint(m_rect.left, m_rect.top), QSize(m_rect.right - m_rect.left, m_rect.bottom - m_rect.top));
					m_RectWidget->SetRect(rect);
					m_RectWidget->SetFunCall(std::function<void(int, int, int, int)>(&MainUI::FinishSelScollArea));
					m_RectWidget->show();
				}
				else
				{
					QRect rect(QPoint(m_rect.left, m_rect.top), QSize(m_rect.right - m_rect.left, m_rect.bottom - m_rect.top));
					m_RectWidget->SetRect(rect);
				}
			}
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

/**
* @brief 简单概述
* @brief 滚动区域截屏.
*/
void MainUI::CaptureScrollArea()
{
	HHOOK mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, NULL, 0);
	if (mouseHook == NULL) 
	{
		// 处理错误
	}

	bFinish = false;

	// 消息循环
	MSG msg;
	while (!bFinish && GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(mouseHook);
}

/**
* @brief 简单概述
* @brief 滚动区域选择完成
*/
void MainUI::FinishSelScollArea(int x, int y, int width, int height)
{
	bFinish = true;
	if (m_RectWidget != NULL)
	{
		delete m_RectWidget;
		m_RectWidget = NULL;
	}

	// 获取鼠标下的窗口句柄
	HWND hwnd = WindowFromPoint(POINT{ x+width/2, y+height /2});

	//while (true)
	//{
	//	SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0); // 垂直滚动示例
	//}
}

/**
* @brief 简单概述
* @brief 颜色拾取
*/
void MainUI::PickScreenColor()
{
	this->setWindowOpacity(0.0); // 设置不透明度(范围：0 到1)
	// 创建一个屏幕对象
	QScreen *screen = QGuiApplication::primaryScreen();
	// 截图
	QPixmap screenshot = screen->grabWindow(0);
	if (m_pColorPicker == NULL)
	{
		m_pColorPicker = new ColorPicker();
		connect(m_pColorPicker, &ColorPicker::FinishEdit, this, &MainUI::FinishPickColor);
	}
	m_pColorPicker->SetPixmap(screenshot);
	m_pColorPicker->show();
}

void MainUI::FinishPickColor(bool state,int r, int g, int b)
{
	if (m_pColorPicker != NULL)
	{
		m_pColorPicker->deleteLater();
		m_pColorPicker = NULL;
	}
	// 设置不透明度(范围：0 到1)
	this->setWindowOpacity(1); 
	if (state)
	{
		if (m_pColorDlg == NULL)
		{
			m_pColorDlg = new ColorDlg();
			connect(m_pColorDlg, &ColorDlg::FinishEdit, this, &MainUI::FinishColorEdit);
		}
		m_pColorDlg->SetColor(r, g, b);
		m_pColorDlg->show();
	}
}

void MainUI::FinishColorEdit(bool result)
{
	if (m_pColorDlg != NULL)
	{
		m_pColorDlg->hide();
		m_pColorDlg->deleteLater();
		m_pColorDlg = NULL;
	}
	if (result)
	{
		PickScreenColor();
	}
}

/**
* @brief 简单概述
* @brief 桌面放大镜功能.
*/
void MainUI::ZoomDeskTop()
{
	this->setWindowOpacity(0.0);
	QScreen *screen = QGuiApplication::primaryScreen();
	QPixmap screenshot = screen->grabWindow(0);
	if (m_pZoomDesktop == NULL)
	{
		m_pZoomDesktop = new ZoomDesktop();
		connect(m_pZoomDesktop, &ZoomDesktop::FinishEdit, this, &MainUI::FinishZoomDeskTop);
	}
	m_pZoomDesktop->SetPixmap(screenshot);
	m_pZoomDesktop->show();
}

/**
* @brief 简单概述
* @brief 结束放大镜操作.
*/
void MainUI::FinishZoomDeskTop()
{
	if (m_pZoomDesktop != NULL)
	{
		m_pZoomDesktop->deleteLater();
		m_pZoomDesktop = NULL;
	}
	this->setWindowOpacity(1.0);
}

/**
* @brief 简单概述
* @brief 初始化用户数据信息
*/
bool MainUI::InitUserInfo(std::string& code,std::string& moneycode)
{
	std::string FileName = SNAPEASE_FOLDER + std::string(SNAPEASE_FILE);
	std::string current_time = GeneralCurTime();

	std::ifstream file(FileName);
	if (!file.is_open())	// 文件不存在,创建文件并初始化数据.
	{
		if (!file.good() && _mkdir(SNAPEASE_FOLDER) != 0)
		{
			LOGOUT_INFO("Create folder fail,folder:%s", SNAPEASE_FOLDER);
		}
		std::ofstream file(FileName, std::ios::out | std::ios::app);

		memset(&m_UserInfo, 0, sizeof(m_UserInfo));
		m_UserInfo.code = CODE_USER_INFO;
		m_UserInfo.id = 0;
		std::string userid = GenerateUniqueIdentifier();
		ReplaceStr(&userid, ":", "");

		std::transform(userid.begin(), userid.end(), userid.begin(),
			[](unsigned char c) { return std::toupper(c); });

		memcpy(m_UserInfo.userid, userid.c_str(), userid.size());
		memcpy(m_UserInfo.usercode, "0", sizeof(m_UserInfo.usercode));
		memcpy(m_UserInfo.installtime, current_time.c_str(), sizeof(m_UserInfo.installtime));
		memcpy(m_UserInfo.recenttime, current_time.c_str(), sizeof(m_UserInfo.recenttime));
		memcpy(m_UserInfo.endtime, "2089-10-09 00:00:00", sizeof(m_UserInfo.endtime));
		m_UserInfo.active = 0;
		m_UserInfo.usecount = 1;
	}
	else
	{
		// 打开二进制文件
		std::ifstream file(FileName, std::ios::binary);
		// 获取文件大小
		file.seekg(0, std::ios::end);
		std::size_t file_size = file.tellg();
		file.seekg(0, std::ios::beg);
		// 创建缓冲区，用于存储文件内容
		char* buffer = new char[file_size];
		// 读取文件内容到缓冲区
		file.read(buffer, file_size);
		memset(&m_UserInfo, 0, sizeof(m_UserInfo));
		memcpy(&m_UserInfo, buffer, sizeof(m_UserInfo));
		delete[] buffer;

		m_UserInfo.code = CODE_USER_INFO;
		m_UserInfo.usecount += 1;
		memcpy(m_UserInfo.recenttime, current_time.c_str(), sizeof(m_UserInfo.recenttime));
	}
	code = m_UserInfo.userid;
	moneycode = m_UserInfo.usercode;
	if (m_UserInfo.active == false && m_UserInfo.endtime <= current_time)	// 试用期已过.
	{
		return true;
	}
	else
	{
		// 存储用户数据到本地文件.
		std::ofstream outfile(FileName, std::ios::binary | std::ios::out);
		outfile.write((const char*)(&m_UserInfo), sizeof(m_UserInfo));
		return false;
	}
}

/**
* @brief 简单概述
* @brief 更新用户数据信息
*/
void MainUI::UpdateUserInfo()
{
	QFuture<void> future = QtConcurrent::run(this, &MainUI::SendUserInfo);
}

/**
* @brief 简单概述
* @brief 向服务器发送
*/
void MainUI::SendUserInfo()
{
	SocketClient client(SERVER_IP0, SERVER_PORT);
	bool connet = client.Connect();
	m_bUpdate = false;
	int loop_count = 0;
	while (!m_bUpdate)
	{
		loop_count += 1;
		if (loop_count > 50)	// 超时退出
		{
			m_bUpdate = true;
			break;
		}
		if (!connet)
		{
			connet = client.Connect();
			Sleep(5000);
			continue;
		}
		int len = client.SendBytes((char*)&m_UserInfo, sizeof(m_UserInfo));
		if (len > 0)
		{
			char recv_msg[SOCKET_MSG_LEN];
			int recv_len = client.ReceiveBytes(recv_msg, SOCKET_MSG_LEN);
			char code = recv_msg[0];
			if (code == CODE_USER_UPDATE && recv_len == sizeof(USERUPDATE))
			{
				USERUPDATE user;
				memset(&user, 0, sizeof(user));
				memcpy(&user, recv_msg, recv_len);

				m_UserInfo.id = user.id;
				memcpy(m_UserInfo.usercode, user.usercode, sizeof(user.usercode));
				memcpy(m_UserInfo.endtime, user.endtime, sizeof(user.endtime));
				if (!m_UserInfo.active)		// 本地未激活，同步云端状态
				{
					m_UserInfo.active = user.active;
				}

				// 存储用户数据到本地文件.
				std::string FileName = SNAPEASE_FOLDER + std::string(SNAPEASE_FILE);
				std::ofstream outfile(FileName, std::ios::binary | std::ios::out);
				outfile.write((const char*)(&m_UserInfo), sizeof(m_UserInfo));

				client.Close();
				m_bUpdate = true;
				break;
			}
		}
		Sleep(5000);
	}
}

/**
* @brief 简单概述
* @brief 检查是否激活
*/
bool MainUI::GetUserActive()
{
	SocketClient client(SERVER_IP0, SERVER_PORT);
	bool connet = client.Connect();
	if (!connet)
		return false;

	int len = client.SendBytes((char*)&m_UserInfo, sizeof(m_UserInfo));
	if (len > 0)
	{
		char recv_msg[SOCKET_MSG_LEN];
		int recv_len = client.ReceiveBytes(recv_msg, SOCKET_MSG_LEN);
		char code = recv_msg[0];
		if (code == CODE_USER_UPDATE && recv_len == sizeof(USERUPDATE))
		{
			USERUPDATE user;
			memset(&user, 0, sizeof(user));
			memcpy(&user, recv_msg, recv_len);

			m_UserInfo.id = user.id;
			memcpy(m_UserInfo.usercode, user.usercode, sizeof(user.usercode));
			memcpy(m_UserInfo.endtime, user.endtime, sizeof(user.endtime));
			m_UserInfo.active = user.active;

			// 存储用户数据到本地文件.
			std::string FileName = SNAPEASE_FOLDER + std::string(SNAPEASE_FILE);
			std::ofstream outfile(FileName, std::ios::binary | std::ios::out);
			outfile.write((const char*)(&m_UserInfo), sizeof(m_UserInfo));

			std::string current_time = GeneralCurTime();
			if (user.active == true || m_UserInfo.endtime >= current_time)
			{
				return true;
			}
		}
	}
	client.Close();
	return false;
}

void MainUI::SetActiveStatus(bool status)
{
	m_UserInfo.active = status;
	// 存储用户数据到本地文件.
	std::string FileName = SNAPEASE_FOLDER + std::string(SNAPEASE_FILE);
	std::ofstream outfile(FileName, std::ios::binary | std::ios::out);
	outfile.write((const char*)(&m_UserInfo), sizeof(m_UserInfo));
}

/**
* @brief 简单概述
* @brief 桌面录屏.
*/
void MainUI::ScreenRecord()
{
	RecordDlg* dlg = new RecordDlg(this);
	dlg->show();
}

void MainUI::BaseConvert()
{
	MemDlg* dlg = new MemDlg();
	dlg->show();
}

void MainUI::ShowColorTable()
{
	ColorList* dlg=new ColorList();
	dlg->show();
}

void MainUI::GenaralQrCode()
{
	QrCode* dlg=new QrCode();
	dlg->show();
}

void MainUI::DoneTextEdit()
{
	TextEdit* dlg = new TextEdit();
	dlg->show();
}

void MainUI::ShowAboutUs()
{
	AboutUs* dlg = new AboutUs();
	dlg->show();
}

void MainUI::TimeConvert()
{
	TimeDlg* dlg = new TimeDlg();
	dlg->show();
}

void MainUI::ParamSetting()
{
	paramdlg* dlg = new paramdlg();
	dlg->show();
}