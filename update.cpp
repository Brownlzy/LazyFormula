#include "update.h"

bool Update::isChecked = false;
bool Update::isError = false;
bool Update::isLatest = false;
bool Update::isUpdated = false;
QString Update::strCRC32 = "";
QUrl Update::BinUrl = "";

Update::Update(QObject* parent, QProgressBar* pa, QLabel* pb, QTextBrowser* pc)
	: QObject(parent), pBar(pa), pLab(pb), pTxt(pc)
{
	Bmanager = new QNetworkAccessManager(this);
}

Update::~Update()
{
}

int Update::CheckUpdate()
{
	pBar->setMaximum(0);
	pLab->setText(tr("Checking Update Information..."));
	QString UpdateInfo = "";
	UpdateInfo = getWebSource(QUrl(UPDATEINFOURL));
	UpdateInfo += "!<<<<<!!";
	qDebug() << UpdateInfo;
	isChecked = true;
	if (UpdateInfo.split("!")[0] == "LazyFormula_OTA_INFO")
	{
		QString info = UpdateInfo.split("!")[1];
		BinUrl = QUrl(info.split("<")[6].replace("[VERSION]", info.split("<")[1]));
		qDebug() << BinUrl;
		strCRC32 = info.split("<")[4];
		pTxt->setPlainText(UpdateInfo.split("!")[2]);
		if (info.split("<")[0].toInt() > VERSIONID)
		{
			pLab->setText("Newest Version: " + info.split("<")[1] + " Size: " + info.split("<")[5]);
			pBar->setMaximum(100);
			pBar->setValue(100);
			qDebug() << "Update::MustUpdate";
			if (info.split("<")[3].toInt())
				return MustUpdate;
			else
				return CantAuto;
		}
		else
		{

			if (info.split("<")[2].toInt() <= VERSIONID)
			{
				isLatest = true;
				pLab->setText(QString("Build Time: ") + RELEASETIME);
				pBar->setMaximum(100);
				pBar->setValue(100);
				qDebug() << "Update::Done";
				return Done;
			}
			else
			{
				pLab->setText("Latest Version: " + info.split("<")[1] + " / Size: " + info.split("<")[5]);
				pBar->setMaximum(100);
				pBar->setValue(100);
				qDebug() << "Update::NotLasted";
				if (info.split("<")[3].toInt())
					return NotLasted;
				else
					return CantAuto;
			}
		}
	}
	else
	{
		isError = true;
		pLab->setText(QString(tr("Build Time: ")) + RELEASETIME);
		pBar->setMaximum(100);
		pBar->setValue(100);
		pTxt->setPlainText(tr("T_T Nothing in here..."));
		qDebug() << "Update::InfoError";
		return InfoError;
	}
}

int Update::doUpdate()
{
	//测试url;
	//BinUrl = "http://dl.360safe.com/drvmgr/guanwang__360DrvMgrInstaller_beta.exe";
	QFileInfo info(BinUrl.path());
	//获取文件名
	//if (fileName.isEmpty()) fileName = "index.html";
	//如果文件名为空，则使用“index.html”，
	QString fileName(qApp->applicationDirPath() + "/LazyFormula.bin");
	file = new QFile(fileName);

	if (!file->open(QIODevice::WriteOnly))
	{   //如果打开文件失败，则删除file，并使file指针为0，然后返回
		pLab->setText(tr("File ""LazyFormula.bin"" Write Error."));
		qDebug() << "file open error";
		delete file;
		file = 0;
		return FileOperateError;
	}
	startRequest(BinUrl);  //进行链接请求
	pLab->setText(tr("Downloading Binary File..."));
	pBar->setMaximum(100);
	pBar->setValue(0);  //进度条的值设为0
	return 0;
}

void Update::startRequest(QUrl url)  //链接请求
{
	qDebug() << url;
	QNetworkRequest Bm_request(url);
	QSslConfiguration Bconf = Bm_request.sslConfiguration();
	Bconf.setPeerVerifyMode(QSslSocket::VerifyNone);
	Bconf.setProtocol(QSsl::TlsV1SslV3);
	Bm_request.setSslConfiguration(Bconf);
	Breply = Bmanager->get(Bm_request);
	qDebug() << Breply->isRunning();
	connect(Breply, SIGNAL(finished()), this, SLOT(httpFinished()));
	//当reply执行完毕后激发httpFinished方法下载完成后
	connect(Breply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
	//当读取replay准备ok后就调用httpReadyRead方法读取内容并写入文件并且根据读取进度更新进度条
	connect(Breply, SIGNAL(downloadProgress(qint64, qint64)),
		this, SLOT(updateDataReadProgress(qint64, qint64)));
}

void Update::httpReadyRead()   //有可用数据
{
	if (file) file->write(Breply->readAll());  //如果文件存在，则写入文件
}

void Update::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
	pBar->setMaximum(totalBytes); //最大值
	pBar->setValue(bytesRead);  //当前值
}

void Update::httpFinished()  //完成下载
{
	pBar->setValue(100);
	pLab->setText(tr("Download Completed."));
	file->flush();
	file->close();
	Breply->deleteLater();
	Breply = 0;
	delete file;
	file = 0;
	pBar->setMaximum(0);
	pLab->setText(tr("Prepare Update Environment..."));
	QTimer::singleShot(500, this, SLOT(Updatefile()));
}

void Update::Updatefile()
{
#ifdef _DEBUG
	qDebug() << strCRC32 << ":" << calcFileCRC(qApp->applicationDirPath() + "/LazyFormula.bin");
#endif // _DEBUG

	if (strCRC32 != calcFileCRC(qApp->applicationDirPath() + "/LazyFormula.bin"))
	{
		pBar->setMaximum(100);
		pBar->setValue(100);
		pLab->setText(tr("CRC32 Check Unpassed."));
		pLab->setToolTip(tr("The validity of the downloaded program cannot be determined. You can try again, or go to LazyFormula.bin in the program directory to manually replace LazyFormula.exe"));
		return;
	}
	std::ofstream fbat;
	if (!fbat)
	{
		pLab->setText(tr("File ""update.bat"" Write Error."));
		return;
	}
	fbat.open(qApp->applicationDirPath().toStdString() + "/update.bat");
	fbat << "@echo off\n";
	fbat << "TIMEOUT /T 3\n";
	fbat << "taskkill /pid " << qApp->applicationPid() << " -t -f\n";
	fbat << "del " << qApp->applicationName().toStdString() << ".exe\n";
	fbat << "ren LazyFormula.bin LazyFormula.exe\n";
	fbat << "start LazyFormula.exe\n";
	fbat << "del update.bat\n";
	fbat.close();
	pLab->setText(tr("Please Restart LazyFormula to Update."));
	isUpdated = true;
	pBar->setMaximum(100);
	pBar->setValue(100);
}

QString getWebSource(QUrl url)
{
	QNetworkAccessManager manager;
	QEventLoop loop;
	QNetworkReply* reply;
	QNetworkRequest m_request(url);
	qDebug() << "Reading html code form " << url;
	QSslConfiguration config;
	QSslConfiguration conf = m_request.sslConfiguration();
	conf.setPeerVerifyMode(QSslSocket::VerifyNone);
	conf.setProtocol(QSsl::TlsV1SslV3);
	m_request.setSslConfiguration(conf);
	reply = manager.get(m_request);
	//请求结束并下载完成后，退出子事件循环
	QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	QTimer::singleShot(15000, &loop, SLOT(quit()));
	//开启子事件循环
	loop.exec();
	QObject::disconnect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	QByteArray codeContent = reply->readAll();
	//return QTextCodec::codecForHtml(codeContent)->toUnicode(codeContent);
	reply->deleteLater();
	return codeContent;
}

QString calcFileCRC(QString fileName)
{
	QString strCRC = "";
	quint32 crc = 0;;
	QFile file(fileName);
	bool result = file.open(QFile::ReadOnly);
	if (result) {
		while (!file.atEnd()) {
			QByteArray raw = file.read(1024 * 1024);
			crc = crc ^ calcCRC32(raw);
		}

		file.close();
		strCRC = QString("%1").arg(crc, 8, 16, QChar('0')).toUpper();
	}

	return strCRC;
}

quint32 calcCRC32(const QByteArray& data)
{
	static const quint32 crc32Table[] =
	{
		0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
		0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
		0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
		0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
		0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
		0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
		0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
		0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
		0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
		0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
		0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
		0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
		0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
		0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
		0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
		0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
		0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
		0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
		0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
		0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
		0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
		0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
		0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
		0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
		0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
		0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
		0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
		0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
		0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
		0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
		0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
		0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
		0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
		0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
		0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
		0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
		0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
		0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
		0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
		0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
		0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
		0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
		0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
	};

	quint32 crc32 = 0xffffffffL;

	for (auto i = 0; i < data.size(); ++i) {
		crc32 = crc32Table[(crc32 ^ data.constData()[i]) & 0xff] ^ (crc32 >> 8);
	}

	return (crc32 ^ 0xffffffffL);
}
