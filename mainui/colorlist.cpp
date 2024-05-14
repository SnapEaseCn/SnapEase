#include "ColorList.h"
#include "kmeans.h"
#include "logtxt.h"
#include <QtConcurrent>
#include <QFileDialog>
#include <QImage>
#include <algorithm>
#include <QtConcurrent>
#include <QClipboard>

#define COLOR_SIZE		24
#define COLOR_SAMPLE	6
#define COLOR_NUM		4

ColorList::ColorList(QWidget *parent):QDialog(parent)
{
	ui.setupUi(this);
	Qt::WindowFlags flags = windowFlags();
	//flags |= Qt::WindowMinMaxButtonsHint;
	flags |= Qt::WindowCloseButtonHint;
	this->setWindowFlags(flags);

	m_image = NULL;
	m_bRun = false;
	m_KMeans = NULL;
	m_timer.setInterval(2000);

	connect(ui.tbSelFile, &QToolButton::clicked, this, &ColorList::SellectFile);
	connect(ui.pbGeneral, &QPushButton::clicked, this, &ColorList::GeneralColor);
	connect(ui.pbClose, &QPushButton::clicked, this, &ColorList::close);

	for (int i = 0; i < COLOR_SIZE; i++)
	{
		QString name = "tbColor_" + QString::number(i+1);
		QToolButton* btn_color =  this->findChild<QToolButton*>(name);
		if (btn_color != NULL)
		{
			map_color[i] = btn_color;
		}
		connect(btn_color, &QToolButton::clicked, this, &ColorList::ColorClicked);
	}

	m_pFutureWatcher = new QFutureWatcher<void>;
	connect(m_pFutureWatcher, SIGNAL(finished()), this, SLOT(FinishCurrentRun()));

	InitCustomColor();
	vec_colors = custom_color[COLOR_NATURE];
	FinishCurrentRun();

	ui.tbRGB->setProperty("id", 0);
	ui.tbDEC->setProperty("id", 1);
	ui.tbHex->setProperty("id", 2);
	connect(ui.tbRGB, &QToolButton::clicked, this, &ColorList::CopyColor);
	connect(ui.tbHex, &QToolButton::clicked, this, &ColorList::CopyColor);
	connect(ui.tbDEC, &QToolButton::clicked, this, &ColorList::CopyColor);

	connect(ui.comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(ColorTopicChange(int)));
	connect(&m_timer, &QTimer::timeout, this, &ColorList::ShowRemainTime);
}

ColorList::~ColorList()
{
}

void ColorList::SellectFile()
{
	QString fileName;
	QFileDialog file_dlg(this);
	file_dlg.setWindowTitle(tr("Select file"));
	file_dlg.setAcceptMode(QFileDialog::AcceptOpen);
	file_dlg.setFileMode(QFileDialog::AnyFile);
	file_dlg.setViewMode(QFileDialog::Detail);

	QStringList filters;
	filters << tr("PNG Format(*.png)") << tr("JPG Format(*.jpg;*.jpeg)") << tr("BMP Format(*.bmp)") << tr("All Files(*)");
	file_dlg.setNameFilters(filters);

	file_dlg.setGeometry(10, 30, 300, 200);
	file_dlg.setDirectory(".");
	if (file_dlg.exec() == QDialog::Accepted)
	{
		fileName = file_dlg.selectedFiles()[0];
	}
	if (!fileName.isEmpty())
	{
		ui.lineEdit->setText(fileName);
		if (m_image != NULL)
		{
			delete m_image;
			m_image = NULL;
		}
		m_image = new QImage();
		if (m_image->load(fileName))
		{
			GeneralColor();
		}
		else
		{
			delete m_image;
			m_image = NULL;
		}
	}
}

void ColorList::GeneralColor()
{
	if (m_image == NULL || m_bRun)
		return;

	m_future = QtConcurrent::run(this, &ColorList::RunGeneralColor);
	m_pFutureWatcher->setFuture(m_future);
	m_bRun = true;
	m_iAlgType = ui.comboBoxAlgType->currentIndex();
	m_IterationsNum = ui.spinBox->value();

	m_iRemainTime = 0;
	m_timer.start();
	ui.labelCacl->setText(tr("Caculating image color table"));
	ui.labelCacl->setVisible(true);
	LOGOUT_DEBUG("start general image color table.");
}

unsigned int Cubehelix(double t, double start = 0.5, double rotations = -1.5, double hue = 1.0, double gamma = 1.0)
{
	double l = pow(t, gamma);
	double a = hue * l * (1 - l) / 2;
	double phi = 2 * M_PI * (start / 3 + 1 + rotations * t);
	double cosphi = cos(phi);
	double sinphi = sin(phi);

	double r = l + a * (-0.14861 * cosphi + 1.78277 * sinphi);
	double g = l + a * (-0.29227 * cosphi - 0.90649 * sinphi);
	double b = l + a * (1.97294 * cosphi);

	unsigned int color = qRgb(r,g,b);
	return color;
}

std::vector<unsigned int> GenerateCubehelixPalette(unsigned int color, int nColors)
{
	std::vector<unsigned int> palette;
	for (int i = 0; i < nColors; ++i) 
	{
		double t = static_cast<double>(i) / (nColors - 1);
		// 调整参数以匹配基础颜色
		QRgb clr(color);
		unsigned int color = Cubehelix(t, qRed(clr), qGreen(clr), qBlue(clr));
		palette.push_back(color);
	}
	return palette;
}

std::vector<unsigned int> FindDominantColor(std::vector<std::pair<unsigned int, int>> &frequency,unsigned short num)
{
	// 将 map 的键添加到 vector 中
	std::vector<unsigned int> sample;
	auto it = frequency.begin();
	for (int i = 0; i < num && it != frequency.end(); ++i, ++it)
	{
		sample.push_back(it->first); 
	}
	return sample;
}

void ColorList::RunGeneralColor()
{
	std::vector<RGB> colors;
	if (m_iAlgType == 0)		// 使用过滤像素算法
	{
		std::map<unsigned int, int> hisgram;
		for (int y = 0; y < m_image->height(); ++y)
		{
			for (int x = 0; x < m_image->width(); ++x)
			{
				QRgb color = m_image->pixel(x, y);
				hisgram[color]++;
			}
		}

		colors.resize(hisgram.size());
		unsigned int ii = 0;
		for (const auto& itr : hisgram)
		{
			if (itr.second > 10)
			{
				colors[ii] = { (unsigned short)qRed(itr.first),(unsigned short)qGreen(itr.first),(unsigned short)qBlue(itr.first) };
				ii++;
			}
		}
		int nn = colors.size() - 1 - ii;
		colors.erase(colors.end() - nn, colors.end());
	}
	else if (m_iAlgType == 1)		// 不使用过滤像素算法
	{
		colors.resize(m_image->width()*m_image->height());
		unsigned int ii = 0;
		for (int y = 0; y < m_image->height(); ++y)
		{
			for (int x = 0; x < m_image->width(); ++x)
			{
				QRgb color = m_image->pixel(x, y);
				colors[ii] = { (unsigned short)qRed(color),(unsigned short)qGreen(color),(unsigned short)qBlue(color) };
				ii++;
			}
		}
	}


	vec_colors.clear();
	if (colors.size() < COLOR_SIZE)
		return;

	auto func = std::bind(&ColorList::UpdateRemainTime, this, std::placeholders::_1);

	// 集群和迭代次数
	if (m_KMeans == NULL)
	{
		m_KMeans = new KMeans(COLOR_SIZE, m_IterationsNum);
	}
	m_KMeans->SetRemainCall(func);
	m_KMeans->run(colors);

	std::vector<RGB> outclr = m_KMeans->getClusterCenters();
	delete m_KMeans;
	m_KMeans = NULL;

	std::string clrinfo;
	vec_colors.resize(outclr.size());
	for (int i = 0; i < outclr.size(); i++)
	{
		vec_colors[i] = qRgb(outclr[i].r, outclr[i].g, outclr[i].b);

		if (i % 2 == 0)
			clrinfo.append("\n");
		else
			clrinfo.append(" ");
		clrinfo.append(std::to_string(vec_colors[i]));
	}
	LOGOUT_DEBUG("Output color table:%s", clrinfo.c_str());
}

void ColorList::FinishCurrentRun()
{
	if (vec_colors.size() < map_color.size())
		return;

	for (int i = 0; i < map_color.size(); i++)
	{
		if (i < vec_colors.size())
		{
			QToolButton* btnclr = map_color[i];
			QRgb clr = vec_colors[i];
			QString ssh = QString("QToolButton{width:38px;height:38px;border:2px solid rgb(100,100,100);background-color:rgb(%0,%1,%2)}QToolButton:checked{width:38px;height:38px;border:2px solid rgb(200,0,0);background-color:rgb(%3,%4,%5)}").arg(qRed(clr)).arg(qGreen(clr)).arg(qBlue(clr)).arg(qRed(clr)).arg(qGreen(clr)).arg(qBlue(clr));
			btnclr->setStyleSheet(ssh);
			btnclr->setProperty("hexcolor", clr);
		}
	}

	if (map_color.size() > 11)
	{
		map_color[10]->setChecked(true);
	}
	
	if (vec_colors.size() > 11)
	{
		unsigned int rgb = vec_colors[10];
		QColor clr(qRed(rgb),qGreen(rgb),qBlue(rgb));

		QString strRgb = QString("%0,%1,%2").arg(clr.red()).arg(clr.green()).arg(clr.blue());
		ui.lineEditRgb->setText(strRgb);

		ui.lineEditHex->setText(clr.name().toUpper());

		int hsv[4];
		clr.getHsv(hsv, hsv + 1, hsv + 2);
		QString strDec = QString("%1,%2,%3").arg(hsv[0]).arg(hsv[1]).arg(hsv[2]);
		ui.lineEditDEC->setText(strDec);
	}
	m_bRun = false;
	ui.labelCacl->setVisible(false);
	m_timer.stop();
}

void ColorList::UpdateRemainTime(unsigned int remain)
{
	m_iRemainTime = remain;
}

void ColorList::ColorClicked()
{
	QToolButton* btn = (QToolButton*)sender();
	unsigned int rgb = btn->property("hexcolor").toUInt();
	
	QColor clr(qRed(rgb), qGreen(rgb), qBlue(rgb));

	QString strRgb = QString("%0,%1,%2").arg(clr.red()).arg(clr.green()).arg(clr.blue());
	ui.lineEditRgb->setText(strRgb);

	ui.lineEditHex->setText(clr.name().toUpper());

	int hsv[4];
	clr.getHsv(hsv, hsv + 1, hsv + 2);
	QString strDec = QString("%1,%2,%3").arg(hsv[0]).arg(hsv[1]).arg(hsv[2]);
	ui.lineEditDEC->setText(strDec);
}

void ColorList::CopyColor()
{
	int id = ((QToolButton*)sender())->property("id").toInt();
	// 创建一个QClipboard对象
	QClipboard *clipboard = QApplication::clipboard();
	if (id == 0)
	{
		// 将文本设置到剪贴板
		clipboard->setText(ui.lineEditRgb->text());
	}
	else if (id == 1)
	{
		// 将文本设置到剪贴板
		clipboard->setText(ui.lineEditDEC->text());
	}
	else if (id == 2)
	{
		// 将文本设置到剪贴板
		clipboard->setText(ui.lineEditHex->text());
	}
}

void ColorList::ShowRemainTime()
{
	if (m_iRemainTime != 0)
	{
		QString tip = QString(tr("Caculating,remain time:%0 sec")).arg(m_iRemainTime);
		ui.labelCacl->setText(tip);
	}
}

void ColorList::ColorTopicChange(int idx)
{
	auto itr = custom_color.find(idx);
	if (itr != custom_color.end() && itr->second.size() >= COLOR_SIZE)
	{
		vec_colors = custom_color[idx];
		FinishCurrentRun();
	}
}

void ColorList::closeEvent(QCloseEvent *event)
{
	if (m_KMeans != NULL)
	{
		m_KMeans->SetExit();
	}
	if (!m_future.isFinished())
	{
		m_pFutureWatcher->waitForFinished();
	}
	this->deleteLater();
	QDialog::closeEvent(event);
}

void ColorList::InitCustomColor()
{
	std::vector<unsigned int> topic;

	/*---------自然生态----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4279722105; topic[0x01] = 4281754409;
	topic[0x02] = 4292996828; topic[0x03] = 4283331088;
	topic[0x04] = 4281107876; topic[0x05] = 4287608438;
	topic[0x06] = 4278551941; topic[0x07] = 4282085706;
	topic[0x08] = 4281029134; topic[0x09] = 4278665051;
	topic[0x0A] = 4280069726; topic[0x0B] = 4279250703;
	topic[0x0C] = 4284128669; topic[0x0D] = 4289714127;
	topic[0x0E] = 4279653963; topic[0x0F] = 4291091826;
	topic[0x10] = 4284322142; topic[0x11] = 4278858799;
	topic[0x12] = 4281956218; topic[0x13] = 4282757571;
	topic[0x14] = 4285174821; topic[0x15] = 4288527152;
	topic[0x16] = 4280043048; topic[0x17] = 4286561989;
	custom_color[COLOR_NATURE] = topic;

	/*---------现代科技----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4278587937; topic[0x01] = 4279976802;
	topic[0x02] = 4290079811; topic[0x03] = 4281549895;
	topic[0x04] = 4279769365; topic[0x05] = 4279315273;
	topic[0x06] = 4287050780; topic[0x07] = 4284765259;
	topic[0x08] = 4280310914; topic[0x09] = 4280820507;
	topic[0x0A] = 4279569158; topic[0x0B] = 4283308044;
	topic[0x0C] = 4291288535; topic[0x0D] = 4278781187;
	topic[0x0E] = 4278586386; topic[0x0F] = 4285969606;
	topic[0x10] = 4280495920; topic[0x11] = 4279180591;
	topic[0x12] = 4286022532; topic[0x13] = 4282671982;
	topic[0x14] = 4282615202; topic[0x15] = 4282856234;
	topic[0x16] = 4281274119; topic[0x17] = 4291014033;
	custom_color[COLOR_TECH] = topic;

	/*---------豪华优雅----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4292860106; topic[0x01] = 4280360473;
	topic[0x02] = 4278650115; topic[0x03] = 4281083681;
	topic[0x04] = 4279109121; topic[0x05] = 4283446818;
	topic[0x06] = 4279833872; topic[0x07] = 4289699979;
	topic[0x08] = 4283517763; topic[0x09] = 4279634435;
	topic[0x0A] = 4278714880; topic[0x0B] = 4280751885;
	topic[0x0C] = 4285688158; topic[0x0D] = 4278387200;
	topic[0x0E] = 4280356356; topic[0x0F] = 4282005039;
	topic[0x10] = 4279373579; topic[0x11] = 4281800453;
	topic[0x12] = 4281934612; topic[0x13] = 4283771663;
	topic[0x14] = 4278190080; topic[0x15] = 4285877805;
	topic[0x16] = 4278979079; topic[0x17] = 4288444499;
	custom_color[COLOR_LUXURY] = topic;

	/*---------青春活力----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4294309067; topic[0x01] = 4290993442;
	topic[0x02] = 4288111955; topic[0x03] = 4288841352;
	topic[0x04] = 4285439145; topic[0x05] = 4283738831;
	topic[0x06] = 4284295981; topic[0x07] = 4292781697;
	topic[0x08] = 4294428259; topic[0x09] = 4284303702;
	topic[0x0A] = 4287315750; topic[0x0B] = 4287088335;
	topic[0x0C] = 4281828528; topic[0x0D] = 4279787159;
	topic[0x0E] = 4279250259; topic[0x0F] = 4294566801;
	topic[0x10] = 4290894032; topic[0x11] = 4285100420;
	topic[0x12] = 4280109675; topic[0x13] = 4289704363;
	topic[0x14] = 4280687405; topic[0x15] = 4291528786;
	topic[0x16] = 4281756292; topic[0x17] = 4293893416;
	custom_color[COLOR_LIVELY] = topic;

	/*---------舒适宁静----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4291736180; topic[0x01] = 4284828173;
	topic[0x02] = 4286811252; topic[0x03] = 4284510028;
	topic[0x04] = 4286079043; topic[0x05] = 4283320109;
	topic[0x06] = 4294112483; topic[0x07] = 4288654492;
	topic[0x08] = 4292794029; topic[0x09] = 4284961075;
	topic[0x0A] = 4287592272; topic[0x0B] = 4288967728;
	topic[0x0C] = 4286336285; topic[0x0D] = 4289892439;
	topic[0x0E] = 4284760095; topic[0x0F] = 4282594336;
	topic[0x10] = 4283642901; topic[0x11] = 4287127086;
	topic[0x12] = 4282982669; topic[0x13] = 4281279509;
	topic[0x14] = 4287584538; topic[0x15] = 4282199316;
	topic[0x16] = 4286266387; topic[0x17] = 4284951826;
	custom_color[COLOR_COMFORTABLE] = topic;

	/*---------传统经典----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4292102453; topic[0x01] = 4288956704;
	topic[0x02] = 4292706897; topic[0x03] = 4288982654;
	topic[0x04] = 4284191599; topic[0x05] = 4291114516;
	topic[0x06] = 4288095244; topic[0x07] = 4293429034;
	topic[0x08] = 4290734896; topic[0x09] = 4286192915;
	topic[0x0A] = 4294634702; topic[0x0B] = 4287196739;
	topic[0x0C] = 4284354055; topic[0x0D] = 4294093657;
	topic[0x0E] = 4280838747; topic[0x0F] = 4282844674;
	topic[0x10] = 4284301859; topic[0x11] = 4294231181;
	topic[0x12] = 4289142572; topic[0x13] = 4281801991;
	topic[0x14] = 4292105030; topic[0x15] = 4280697127;
	topic[0x16] = 4279832834; topic[0x17] = 4278454784;
	custom_color[COLOR_TRADITION] = topic;

	/*---------简约极致----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4286799934; topic[0x01] = 4283845951;
	topic[0x02] = 4284761900; topic[0x03] = 4285355323;
	topic[0x04] = 4282722835; topic[0x05] = 4284503882;
	topic[0x06] = 4281017371; topic[0x07] = 4282335818;
	topic[0x08] = 4287334012; topic[0x09] = 4281806113;
	topic[0x0A] = 4285424210; topic[0x0B] = 4289039721;
	topic[0x0C] = 4280424717; topic[0x0D] = 4287591764;
	topic[0x0E] = 4279700483; topic[0x0F] = 4294506223;
	topic[0x10] = 4281539848; topic[0x11] = 4283775263;
	topic[0x12] = 4291213474; topic[0x13] = 4293254087;
	topic[0x14] = 4282660648; topic[0x15] = 4289109391;
	topic[0x16] = 4283318835; topic[0x17] = 4286149222;
	custom_color[COLOR_SIMPLE] = topic;

	/*---------浪漫高雅----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4280359706; topic[0x01] = 4284501569;
	topic[0x02] = 4286203956; topic[0x03] = 4284691494;
	topic[0x04] = 4279571735; topic[0x05] = 4283644978;
	topic[0x06] = 4280950300; topic[0x07] = 4282659885;
	topic[0x08] = 4282326808; topic[0x09] = 4290226061;
	topic[0x0A] = 4287131739; topic[0x0B] = 4279243285;
	topic[0x0C] = 4282919200; topic[0x0D] = 4281214499;
	topic[0x0E] = 4280358420; topic[0x0F] = 4284032795;
	topic[0x10] = 4289027384; topic[0x11] = 4293844408;
	topic[0x12] = 4286986786; topic[0x13] = 4279834649;
	topic[0x14] = 4281737757; topic[0x15] = 4291791958;
	topic[0x16] = 4281145878; topic[0x17] = 4281936678;
	custom_color[COLOR_ROMANTIC] = topic;

	/*---------高尚文明----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4292595132; topic[0x01] = 4283907633;
	topic[0x02] = 4288974202; topic[0x03] = 4284493854;
	topic[0x04] = 4279374613; topic[0x05] = 4279570189;
	topic[0x06] = 4281147420; topic[0x07] = 4290751384;
	topic[0x08] = 4282658856; topic[0x09] = 4281935648;
	topic[0x0A] = 4284108101; topic[0x0B] = 4280555798;
	topic[0x0C] = 4288375626; topic[0x0D] = 4280030226;
	topic[0x0E] = 4285619794; topic[0x0F] = 4280750602;
	topic[0x10] = 4279834650; topic[0x11] = 4282334267;
	topic[0x12] = 4282261267; topic[0x13] = 4280426529;
	topic[0x14] = 4287001703; topic[0x15] = 4281150251;
	topic[0x16] = 4286204723; topic[0x17] = 4279045132;
	custom_color[COLOR_CULTURE] = topic;

	/*---------强烈醒目----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4278584066; topic[0x01] = 4284105267;
	topic[0x02] = 4286945425; topic[0x03] = 4294239146;
	topic[0x04] = 4278190592; topic[0x05] = 4279047705;
	topic[0x06] = 4283065191; topic[0x07] = 4279314226;
	topic[0x08] = 4278454541; topic[0x09] = 4290255116;
	topic[0x0A] = 4292767003; topic[0x0B] = 4278255616;
	topic[0x0C] = 4281411361; topic[0x0D] = 4292914022;
	topic[0x0E] = 4279502083; topic[0x0F] = 4283240712;
	topic[0x10] = 4278256131; topic[0x11] = 4278190080;
	topic[0x12] = 4280945415; topic[0x13] = 4278190080;
	topic[0x14] = 4280697164; topic[0x15] = 4288039456;
	topic[0x16] = 4286060557; topic[0x17] = 4288181843;
	custom_color[COLOR_VEHEMENT] = topic;

	/*---------分裂互补----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4278783765; topic[0x01] = 4285032287;
	topic[0x02] = 4293848030; topic[0x03] = 4278715908;
	topic[0x04] = 4280699748; topic[0x05] = 4293841571;
	topic[0x06] = 4281081104; topic[0x07] = 4284559124;
	topic[0x08] = 4281808431; topic[0x09] = 4279180585;
	topic[0x0A] = 4292384336; topic[0x0B] = 4283649859;
	topic[0x0C] = 4279447366; topic[0x0D] = 4292991427;
	topic[0x0E] = 4289899932; topic[0x0F] = 4292785025;
	topic[0x10] = 4286093222; topic[0x11] = 4287516452;
	topic[0x12] = 4289884737; topic[0x13] = 4286939251;
	topic[0x14] = 4281822865; topic[0x15] = 4291609516;
	topic[0x16] = 4289304188; topic[0x17] = 4290365643;
	custom_color[COLOR_SPLIT] = topic;

	/*---------温暖阳光----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4285281299; topic[0x01] = 4283596958;
	topic[0x02] = 4287656540; topic[0x03] = 4289424952;
	topic[0x04] = 4286490552; topic[0x05] = 4289815068;
	topic[0x06] = 4283509272; topic[0x07] = 4290760651;
	topic[0x08] = 4291799937; topic[0x09] = 4286071843;
	topic[0x0A] = 4292976188; topic[0x0B] = 4288253575;
	topic[0x0C] = 4294367890; topic[0x0D] = 4283913560;
	topic[0x0E] = 4287388214; topic[0x0F] = 4285550650;
	topic[0x10] = 4291460647; topic[0x11] = 4293836129;
	topic[0x12] = 4287185682; topic[0x13] = 4291005520;
	topic[0x14] = 4281671962; topic[0x15] = 4284234536;
	topic[0x16] = 4294702029; topic[0x17] = 4288107296;
	custom_color[COLOR_WARM] = topic;

	/*---------高冷自信----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4281555572; topic[0x01] = 4283390812;
	topic[0x02] = 4279899675; topic[0x03] = 4279507497;
	topic[0x04] = 4280561729; topic[0x05] = 4281418322;
	topic[0x06] = 4279641661; topic[0x07] = 4284253077;
	topic[0x08] = 4282793523; topic[0x09] = 4281355876;
	topic[0x0A] = 4288264378; topic[0x0B] = 4286094758;
	topic[0x0C] = 4280296496; topic[0x0D] = 4288653982;
	topic[0x0E] = 4282082431; topic[0x0F] = 4283657591;
	topic[0x10] = 4285758324; topic[0x11] = 4282938254;
	topic[0x12] = 4279372556; topic[0x13] = 4287009675;
	topic[0x14] = 4278980378; topic[0x15] = 4293917158;
	topic[0x16] = 4291417282; topic[0x17] = 4280105042;
	custom_color[COLOR_COLD] = topic;

	/*---------熏衣草香----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4286470938; topic[0x01] = 4279048231;
	topic[0x02] = 4279904280; topic[0x03] = 4283651655;
	topic[0x04] = 4282679663; topic[0x05] = 4294303903;
	topic[0x06] = 4289970318; topic[0x07] = 4288177559;
	topic[0x08] = 4291071269; topic[0x09] = 4282336030;
	topic[0x0A] = 4290088044; topic[0x0B] = 4285115271;
	topic[0x0C] = 4279906094; topic[0x0D] = 4286935112;
	topic[0x0E] = 4280164167; topic[0x0F] = 4293048928;
	topic[0x10] = 4282200422; topic[0x11] = 4281873984;
	topic[0x12] = 4284630409; topic[0x13] = 4281031769;
	topic[0x14] = 4279715142; topic[0x15] = 4286938732;
	topic[0x16] = 4285484392; topic[0x17] = 4291925170;
	custom_color[COLOR_LAVENDER] = topic;

	/*---------宁静天空----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4285953184; topic[0x01] = 4278200163;
	topic[0x02] = 4278858353; topic[0x03] = 4280433009;
	topic[0x04] = 4278266471; topic[0x05] = 4278265951;
	topic[0x06] = 4278528866; topic[0x07] = 4279185256;
	topic[0x08] = 4291616232; topic[0x09] = 4283716221;
	topic[0x0A] = 4280111768; topic[0x0B] = 4282877632;
	topic[0x0C] = 4282144144; topic[0x0D] = 4278462045;
	topic[0x0E] = 4281427881; topic[0x0F] = 4280698242;
	topic[0x10] = 4278921821; topic[0x11] = 4279517573;
	topic[0x12] = 4278190080; topic[0x13] = 4289764532;
	topic[0x14] = 4279971682; topic[0x15] = 4278529386;
	topic[0x16] = 4286427353; topic[0x17] = 4279187577;
	custom_color[COLOR_SERENITY] = topic;

	/*---------户外瑜珈----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4292583035; topic[0x01] = 4291122756;
	topic[0x02] = 4291846480; topic[0x03] = 4284441951;
	topic[0x04] = 4294415196; topic[0x05] = 4294172847;
	topic[0x06] = 4278461750; topic[0x07] = 4286790976;
	topic[0x08] = 4294822011; topic[0x09] = 4288889667;
	topic[0x0A] = 4293490256; topic[0x0B] = 4290272338;
	topic[0x0C] = 4282207313; topic[0x0D] = 4280560436;
	topic[0x0E] = 4294814055; topic[0x0F] = 4287135340;
	topic[0x10] = 4289834638; topic[0x11] = 4289875272;
	topic[0x12] = 4293755734; topic[0x13] = 4294900702;
	topic[0x14] = 4292633670; topic[0x15] = 4292767567;
	topic[0x16] = 4284033837; topic[0x17] = 4291589982;
	custom_color[COLOR_OUTDOORS] = topic;

	/*---------诱人香水----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4279177494; topic[0x01] = 4285743655;
	topic[0x02] = 4283833101; topic[0x03] = 4279833362;
	topic[0x04] = 4293247890; topic[0x05] = 4283580469;
	topic[0x06] = 4291129905; topic[0x07] = 4294635213;
	topic[0x08] = 4285685583; topic[0x09] = 4281740328;
	topic[0x0A] = 4285736975; topic[0x0B] = 4278979340;
	topic[0x0C] = 4283378972; topic[0x0D] = 4281343769;
	topic[0x0E] = 4282259215; topic[0x0F] = 4289432685;
	topic[0x10] = 4278388748; topic[0x11] = 4278586387;
	topic[0x12] = 4280163358; topic[0x13] = 4288365335;
	topic[0x14] = 4292911705; topic[0x15] = 4280882193;
	topic[0x16] = 4280224775; topic[0x17] = 4288111421;
	custom_color[COLOR_PERFUME] = topic;

	/*---------健康养生----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4280505136; topic[0x01] = 4288400264;
	topic[0x02] = 4289455280; topic[0x03] = 4288464493;
	topic[0x04] = 4293255094; topic[0x05] = 4290236339;
	topic[0x06] = 4281953084; topic[0x07] = 4293721060;
	topic[0x08] = 4292735957; topic[0x09] = 4291291563;
	topic[0x0A] = 4286951255; topic[0x0B] = 4283467043;
	topic[0x0C] = 4290835654; topic[0x0D] = 4289846165;
	topic[0x0E] = 4284913512; topic[0x0F] = 4286230910;
	topic[0x10] = 4291015816; topic[0x11] = 4285308474;
	topic[0x12] = 4279450405; topic[0x13] = 4291617734;
	topic[0x14] = 4283335504; topic[0x15] = 4281361431;
	topic[0x16] = 4288329113; topic[0x17] = 4287418525;
	custom_color[COLOR_HEALTHY] = topic;

	/*---------星空蓝光----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4278663538; topic[0x01] = 4278798467;
	topic[0x02] = 4279249498; topic[0x03] = 4279568420;
	topic[0x04] = 4279313488; topic[0x05] = 4279245893;
	topic[0x06] = 4279374902; topic[0x07] = 4278725987;
	topic[0x08] = 4279569707; topic[0x09] = 4279441984;
	topic[0x0A] = 4279506746; topic[0x0B] = 4279571251;
	topic[0x0C] = 4279272636; topic[0x0D] = 4281449438;
	topic[0x0E] = 4279569192; topic[0x0F] = 4279443529;
	topic[0x10] = 4278918733; topic[0x11] = 4279048000;
	topic[0x12] = 4278869660; topic[0x13] = 4278789207;
	topic[0x14] = 4279318120; topic[0x15] = 4279308337;
	topic[0x16] = 4279570478; topic[0x17] = 4288016625;
	custom_color[COLOR_SKY] = topic;

	/*---------醒目柑橘----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4285036408; topic[0x01] = 4287703570;
	topic[0x02] = 4289765958; topic[0x03] = 4281154354;
	topic[0x04] = 4288179725; topic[0x05] = 4283783697;
	topic[0x06] = 4279508506; topic[0x07] = 4283391836;
	topic[0x08] = 4283966217; topic[0x09] = 4291321861;
	topic[0x0A] = 4286484369; topic[0x0B] = 4278782733;
	topic[0x0C] = 4291183678; topic[0x0D] = 4279112467;
	topic[0x0E] = 4289442460; topic[0x0F] = 4280100392;
	topic[0x10] = 4282075975; topic[0x11] = 4281145353;
	topic[0x12] = 4293189048; topic[0x13] = 4285957426;
	topic[0x14] = 4292987767; topic[0x15] = 4280499470;
	topic[0x16] = 4292780041; topic[0x17] = 4293709113;
	custom_color[COLOR_CITRUS] = topic;

	/*---------浓烈红酒----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4283322198; topic[0x01] = 4287758598;
	topic[0x02] = 4279435288; topic[0x03] = 4286382088;
	topic[0x04] = 4294604338; topic[0x05] = 4280352789;
	topic[0x06] = 4294695075; topic[0x07] = 4281281326;
	topic[0x08] = 4290196012; topic[0x09] = 4282581006;
	topic[0x0A] = 4278845466; topic[0x0B] = 4281401362;
	topic[0x0C] = 4294121481; topic[0x0D] = 4283367436;
	topic[0x0E] = 4280811540; topic[0x0F] = 4281008143;
	topic[0x10] = 4285079336; topic[0x11] = 4292181861;
	topic[0x12] = 4285136905; topic[0x13] = 4291953413;
	topic[0x14] = 4284153866; topic[0x15] = 4281925648;
	topic[0x16] = 4279959832; topic[0x17] = 4289659142;
	custom_color[COLOR_REDWINE] = topic;

	/*---------农夫市集----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4284973706; topic[0x01] = 4291406941;
	topic[0x02] = 4294506725; topic[0x03] = 4283194666;
	topic[0x04] = 4292005041; topic[0x05] = 4292447784;
	topic[0x06] = 4293912258; topic[0x07] = 4287244313;
	topic[0x08] = 4282079077; topic[0x09] = 4286881189;
	topic[0x0A] = 4280034094; topic[0x0B] = 4288381010;
	topic[0x0C] = 4283051302; topic[0x0D] = 4290334749;
	topic[0x0E] = 4290556560; topic[0x0F] = 4280959043;
	topic[0x10] = 4284575587; topic[0x11] = 4288386682;
	topic[0x12] = 4288918966; topic[0x13] = 4285155898;
	topic[0x14] = 4286478640; topic[0x15] = 4293578885;
	topic[0x16] = 4292404706; topic[0x17] = 4290432465;
	custom_color[COLOR_FAIR] = topic;

	/*---------缤纷美学----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4283182103; topic[0x01] = 4284355080;
	topic[0x02] = 4279778640; topic[0x03] = 4291910434;
	topic[0x04] = 4280390328; topic[0x05] = 4281743152;
	topic[0x06] = 4285234045; topic[0x07] = 4288115533;
	topic[0x08] = 4279576107; topic[0x09] = 4279111949;
	topic[0x0A] = 4279919748; topic[0x0B] = 4283273515;
	topic[0x0C] = 4281212432; topic[0x0D] = 4289500274;
	topic[0x0E] = 4282798161; topic[0x0F] = 4292327825;
	topic[0x10] = 4293321942; topic[0x11] = 4293306668;
	topic[0x12] = 4280893713; topic[0x13] = 4287278265;
	topic[0x14] = 4285488196; topic[0x15] = 4287047700;
	topic[0x16] = 4284236584; topic[0x17] = 4287779105;
	custom_color[COLOR_AESTHETICS] = topic;

	/*---------漂亮粉红----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4287708481; topic[0x01] = 4290011233;
	topic[0x02] = 4293633191; topic[0x03] = 4293452765;
	topic[0x04] = 4292004291; topic[0x05] = 4285076508;
	topic[0x06] = 4293904581; topic[0x07] = 4292172915;
	topic[0x08] = 4292908183; topic[0x09] = 4282390804;
	topic[0x0A] = 4287846229; topic[0x0B] = 4285416770;
	topic[0x0C] = 4290738809; topic[0x0D] = 4287111984;
	topic[0x0E] = 4294570473; topic[0x0F] = 4284558891;
	topic[0x10] = 4292663249; topic[0x11] = 4288509554;
	topic[0x12] = 4294498516; topic[0x13] = 4292640391;
	topic[0x14] = 4290001999; topic[0x15] = 4291408300;
	topic[0x16] = 4290745745; topic[0x17] = 4293899959;
	custom_color[COLOR_PINK] = topic;

	/*---------珊瑚绿植----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4284327290; topic[0x01] = 4282275338;
	topic[0x02] = 4285771910; topic[0x03] = 4282092127;
	topic[0x04] = 4289643411; topic[0x05] = 4292114768;
	topic[0x06] = 4283394355; topic[0x07] = 4278924820;
	topic[0x08] = 4284512018; topic[0x09] = 4283275116;
	topic[0x0A] = 4279391773; topic[0x0B] = 4280034570;
	topic[0x0C] = 4293700229; topic[0x0D] = 4282207525;
	topic[0x0E] = 4287931737; topic[0x0F] = 4285277201;
	topic[0x10] = 4285563969; topic[0x11] = 4279048198;
	topic[0x12] = 4280956946; topic[0x13] = 4279396411;
	topic[0x14] = 4287470366; topic[0x15] = 4278651651;
	topic[0x16] = 4289284137; topic[0x17] = 4281040209;
	custom_color[COLOR_CORAL] = topic;

	/*---------绿色美学----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4290893994; topic[0x01] = 4280828187;
	topic[0x02] = 4279840019; topic[0x03] = 4278720262;
	topic[0x04] = 4283786291; topic[0x05] = 4280568071;
	topic[0x06] = 4282411276; topic[0x07] = 4284054604;
	topic[0x08] = 4286881904; topic[0x09] = 4282012198;
	topic[0x0A] = 4279051524; topic[0x0B] = 4285893464;
	topic[0x0C] = 4281742352; topic[0x0D] = 4293785558;
	topic[0x0E] = 4279645189; topic[0x0F] = 4292339646;
	topic[0x10] = 4288134210; topic[0x11] = 4284911391;
	topic[0x12] = 4278455042; topic[0x13] = 4289775762;
	topic[0x14] = 4280904767; topic[0x15] = 4288263556;
	topic[0x16] = 4279181326; topic[0x17] = 4279321120;
	custom_color[COLOR_GREENAESTHETICS] = topic;

	/*---------光彩百合----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4294696358; topic[0x01] = 4291876817;
	topic[0x02] = 4293898863; topic[0x03] = 4282674303;
	topic[0x04] = 4293191906; topic[0x05] = 4291909712;
	topic[0x06] = 4291990903; topic[0x07] = 4289576641;
	topic[0x08] = 4286748579; topic[0x09] = 4293496355;
	topic[0x0A] = 4294703853; topic[0x0B] = 4292524452;
	topic[0x0C] = 4293514431; topic[0x0D] = 4287832846;
	topic[0x0E] = 4286271032; topic[0x0F] = 4288050797;
	topic[0x10] = 4291510542; topic[0x11] = 4294428812;
	topic[0x12] = 4294764991; topic[0x13] = 4283765260;
	topic[0x14] = 4289635997; topic[0x15] = 4294767831;
	topic[0x16] = 4278850837; topic[0x17] = 4280826685;
	custom_color[COLOR_RADIANTLILY] = topic;

	/*---------酷炫群体----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4278988862; topic[0x01] = 4288850858;
	topic[0x02] = 4280447890; topic[0x03] = 4278453253;
	topic[0x04] = 4278255873; topic[0x05] = 4292271061;
	topic[0x06] = 4278913804; topic[0x07] = 4294704120;
	topic[0x08] = 4278190080; topic[0x09] = 4279441177;
	topic[0x0A] = 4280755236; topic[0x0B] = 4291477423;
	topic[0x0C] = 4281939510; topic[0x0D] = 4290037448;
	topic[0x0E] = 4282927692; topic[0x0F] = 4278190080;
	topic[0x10] = 4283920252; topic[0x11] = 4288778121;
	topic[0x12] = 4279390309; topic[0x13] = 4284898135;
	topic[0x14] = 4278190080; topic[0x15] = 4285633956;
	topic[0x16] = 4286805872; topic[0x17] = 4278190080;
	custom_color[COLOR_COOL] = topic;

	/*---------时尚服饰----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4278913029; topic[0x01] = 4292002473;
	topic[0x02] = 4279896834; topic[0x03] = 4294242274;
	topic[0x04] = 4279439371; topic[0x05] = 4284757014;
	topic[0x06] = 4287454003; topic[0x07] = 4289297747;
	topic[0x08] = 4289632895; topic[0x09] = 4282065168;
	topic[0x0A] = 4280292105; topic[0x0B] = 4286936160;
	topic[0x0C] = 4283051546; topic[0x0D] = 4281410584;
	topic[0x0E] = 4280557336; topic[0x0F] = 4280949262;
	topic[0x10] = 4281478179; topic[0x11] = 4283179016;
	topic[0x12] = 4283581235; topic[0x13] = 4281602820;
	topic[0x14] = 4285159750; topic[0x15] = 4279965714;
	topic[0x16] = 4285286185; topic[0x17] = 4282463785;
	custom_color[COLOR_COSTUMES] = topic;

	/*---------惬意旅途----------*/
	topic.clear();	topic.resize(24);
	topic[0x00] = 4283862544; topic[0x01] = 4287547305;
	topic[0x02] = 4291156014; topic[0x03] = 4280907051;
	topic[0x04] = 4288654442; topic[0x05] = 4278670373;
	topic[0x06] = 4288132365; topic[0x07] = 4294703314;
	topic[0x08] = 4285839773; topic[0x09] = 4289451954;
	topic[0x0A] = 4279066436; topic[0x0B] = 4292075440;
	topic[0x0C] = 4285635401; topic[0x0D] = 4284899879;
	topic[0x0E] = 4291152509; topic[0x0F] = 4280387170;
	topic[0x10] = 4278664247; topic[0x11] = 4282270998;
	topic[0x12] = 4282356589; topic[0x13] = 4278723873;
	topic[0x14] = 4278600721; topic[0x15] = 4283934085;
	topic[0x16] = 4292866249; topic[0x17] = 4290437568;
	custom_color[COLOR_TRAVEL] = topic;
}