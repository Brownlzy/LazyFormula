#pragma once

#include <QObject>
#include <QUrl>
#include <QDesktopServices>
#include <QTimer>
#include <QtNetwork>
#include <QEventLoop>
#include <QProgressBar>
#include <QLabel>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QApplication>
#include <QTextBrowser>
#include <fstream>
#include "appinfo.h"

class Update : public QObject
{
	Q_OBJECT
private:
	QProgressBar* pBar;
	QLabel* pLab;
	QTextBrowser* pTxt;
	QFile* file;  //文件指针
	QNetworkAccessManager* Bmanager;//请求网络的对象
	QNetworkReply* Breply;//出来返回结果的对象

public:
	static bool isChecked;
	static bool isError;
	static bool isLatest;
	static bool isUpdated;
	static const int Done = 0;
	static const int InfoError = 1;
	static const int DownFail = 2;
	static const int Crc32Error = 3;
	static const int FileOperateError = 4;
	static const int NotLasted = 5;
	static const int MustUpdate = 6;
	static const int CantAuto = 7;
	static QString strCRC32;
	static QUrl BinUrl;   //存储网络地址
	Update(QObject* parent, QProgressBar* pa, QLabel* pb, QTextBrowser* pc);
	~Update();
	int CheckUpdate();
	int doUpdate();
	void startRequest(QUrl url); //请求链接

private slots:
	void Updatefile();
	void httpFinished();  //完成下载后的处理
	void httpReadyRead();  //接收到数据时的处理
	void updateDataReadProgress(qint64, qint64); //更新进度条
};

QString getWebSource(QUrl url);
QString calcFileCRC(QString fileName);
quint32 calcCRC32(const QByteArray& data);

/*
* UpdateInfo Format:
* LazyFormula_OTA_INFO!10<0.1.0<10<1<XXXXXXXX<XXKB<https://Brownlzy.github.io/LazyFormula/LazyFormulav[VERSION]_exe !UpdateInfo!
*/

/*
void Settings::btnDoUpdate()
{
	if (pUpdate->isChecked && pUpdate->isError)
	{
		pUpdate->isChecked = false;
		ui.btnUpdate->setEnabled(false);
	}
	else if (ui.btnUpdate->text() == tr("Update"))
	{
		ui.btnUpdate->setEnabled(false);
		pUpdate->doUpdate();
	}
	else
	{
		ui.btnUpdate->setEnabled(false);
		int rect = pUpdate->CheckUpdate();
		if (rect == 6 || rect == 5)
		{
			ui.btnUpdate->setText(tr("Update"));
		}
		ui.btnUpdate->setEnabled(true);
	}
}
*/
