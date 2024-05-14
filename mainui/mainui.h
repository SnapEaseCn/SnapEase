#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <windows.h>
#include <QWidget>
#include <QSystemTrayIcon>
#include "ui_mainui.h"
#include <time.h>
#include <queue>
#include "util.h"
#include "public.h"

class pngwriter;
class QCloseEvent;
class ImageReader;
class ImageWriter;
class ImageEdit;
class VoiceEdit;
class QMenu;
class QTimer;
class QPainter;
class ScreenDlg;
class ColorPicker;
class ColorDlg;
class ScreenActive;
class ZoomDesktop;
class RectDlg;
class MainUI : public QWidget
{
    Q_OBJECT

public:
    explicit MainUI(QWidget *parent = 0);
    ~MainUI();

	/**
	* @brief 简单概述
	* @brief 初始化用户数据信息
	*/
	bool InitUserInfo(std::string& code,std::string& moneycode);

	/**
	* @brief 简单概述
	* @brief 更新用户数据信息
	*/
	void UpdateUserInfo();

	/**
	* @brief 简单概述
	* @brief 检查是否激活
	*/
	bool GetUserActive();

	/**
	* @brief 简单概述
	* @brief 更新激活状态
	*/
	void SetActiveStatus(bool status);

	/**
	* @brief 简单概述
	* @brief 存储系统托盘对象
	*/
	void SetTrayIcon(QSystemTrayIcon* ico) { m_TrayIcon = ico; }


	/**
	* @brief 简单概述
	* @brief 滚动区域选择完成
	*/
	static void FinishSelScollArea(int x, int y, int width, int height);

private:
    /**
    * @brief 简单概述
    * @brief 信号连接
    */
	void ConnectSignal();

    /**
    * @brief 简单概述
    * @brief 重载关闭窗口事件
    */
	void closeEvent(QCloseEvent *evt);

private slots:
	/**
	* @brief 简单概述
	* @brief 打开图像
	*/
	void OpenImage();

	/**
	* @brief 简单概述
	* @brief 托盘图标被点击.
	*/
	void TrayIconActivated(QSystemTrayIcon::ActivationReason reason);

	/**
	* @brief 简单概述
	* @brief 最小化按钮被点击.
	*/
	void MinBtnClick();

	/**
	* @brief 简单概述
	* @brief 截取目标窗口.
	*/
	void CaptureTargetWindow();

	/**
	* @brief 简单概述
	* @brief 截取目标区域.
	*/
	void CaptureRegion();

	/**
	* @brief 简单概述
	* @brief 截取全屏
	*/
	void CaptureFullScreen();

	/**
	* @brief 简单概述
	* @brief 文本转语音.
	*/
	void Text2Voice();

	/**
	* @brief 简单概述
	* @brief 关闭文本转语音.
	*/
	void VoiceEditFinish();

	/**
	* @brief 简单概述
	* @brief 关闭文本转语音.
	*/
	void CloseUI();

	/**
	* @brief 简单概述
	* @brief 定时器事件.
	*/
	void OnFrameUpdate();

	/**
	* @brief 简单概述
	* @brief 选择框选区域完成.
	*/
	void FinishSelRect(QRect);

	/**
	* @brief 简单概述
	* @brief 编辑图像完成.
	*/
	void FinishImageEdit();

	/**
	* @brief 简单概述
	* @brief 滚动区域截屏.
	*/
	void CaptureScrollArea();

	/**
	* @brief 简单概述
	* @brief 颜色拾取
	*/
	void PickScreenColor();

	/**
	* @brief 简单概述
	* @brief 颜色拾取
	*/
	void FinishPickColor(bool,int r, int g, int b);

	/**
	* @brief 简单概述
	* @brief 颜色拾取
	*/
	void FinishColorEdit(bool);

	/**
	* @brief 简单概述
	* @brief 桌面放大镜功能.
	*/
	void ZoomDeskTop();

	/**
	* @brief 简单概述
	* @brief 结束放大镜操作.
	*/
	void FinishZoomDeskTop();

	/**
	* @brief 简单概述
	* @brief 桌面录屏.
	*/
	void ScreenRecord();

	/**
	* @brief 简单概述
	* @brief 进制转换.
	*/
	void BaseConvert();

	/**
	* @brief 简单概述
	* @brief 颜色卡提取.
	*/
	void ShowColorTable();

	/**
	* @brief 简单概述
	* @brief 生成二维码
	*/
	void GenaralQrCode();

	/**
	* @brief 简单概述
	* @brief 文本编辑助手
	*/
	void DoneTextEdit();

	/**
	* @brief 简单概述
	* @brief 关于我们
	*/
	void ShowAboutUs();

	/**
	* @brief 简单概述
	* @brief 时间戳转换
	*/
	void TimeConvert();

	/**
	* @brief 简单概述
	* @brief 参数设置
	*/
	void ParamSetting();

private:
	/**
	* @brief 简单概述
	* @brief 鼠标移动事件
	*/
	void mouseMoveEvent(QMouseEvent * event);

	/**
	* @brief 简单概述
	* @brief 鼠标按下事件
	*/
	void mousePressEvent(QMouseEvent * event);

	/**
	* @brief 简单概述
	* @brief 鼠标按下事件
	*/
	void mouseReleaseEvent(QMouseEvent * event);

	/**
	* @brief 简单概述
	* @brief 重绘事件.
	*/
	void paintEvent(QPaintEvent *event) override;

	/**
	* @brief 简单概述
	* @brief 选取打开文件路径，精确到文件
	*/
	QString SelectFileNamePath();

	/**
	* @brief 简单概述
	* @brief 选取保存文件路径，精确到文件
	*/
	QString SelectSavePath();

	/**
	* @brief 简单概述
	* @brief 按键事件
	*/
	void keyPressEvent(QKeyEvent *event) override;

	/**
	* @brief 简单概述
	* @brief 各种实时事件
	*/
	bool eventFilter(QObject *obj, QEvent *event) override;

	/**
	* @brief 简单概述
	* @brief 向服务器发送
	*/
	void SendUserInfo();

private:
    Ui::MainUI ui;

	/**
	* @brief 简单概述
	* @brief 日志输出定时器
	*/
	QString		m_sLogText;
	QPoint		mouseQPoint;
	QMenu*		m_MenuMore;

	/**
	* @brief 简单概述
	* @brief 图像编辑器
	*/
	ImageEdit*		m_pImageEdit;

	/**
	* @brief 简单概述
	* @brief 语音编辑器
	*/
	VoiceEdit*			m_pVoiceEdit;

	OPERATION_MOUSE		m_OperMouse;
	QTimer*				m_timer;
	ScreenDlg*			m_pScreenDlg;
	ColorPicker*		m_pColorPicker;
	ColorDlg*			m_pColorDlg;
	ScreenActive*		m_pScreenActive;
	ZoomDesktop*		m_pZoomDesktop;

	/**
	* @brief 简单概述
	* @brief 用户数据信息
	*/
	USERINFO			m_UserInfo;

	/**
	* @brief 简单概述
	* @brief 更新结果
	*/
	bool				m_bUpdate;

	QSystemTrayIcon*	m_TrayIcon;
};

#endif // MAINWINDOW_H
