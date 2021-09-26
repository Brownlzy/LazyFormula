#include "LazyFormula.h"
#include <QApplication>
#include <QFileInfo>
#include <QProcess>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	QFileInfo file(qApp->applicationDirPath() + "/update.bat");
	if (file.exists() == true)
	{
		QProcess::startDetached(qApp->applicationDirPath() + "/update.bat");
		QTimer::singleShot(0, qApp, &QCoreApplication::quit);
	}
	else
	{
		QFileInfo cfgfile(qApp->applicationDirPath() + "/config.ini");
		if (cfgfile.exists() != true)
		{
			//TODO:生成默认配置文件
		}
		QTranslator translator;
		translator.load(":/language/lazyformula_zh.qm");
		qApp->installTranslator(&translator);
		LazyFormula w;
		w.setWindowIcon(QIcon(":/pic/ico"));
		w.show();
		return a.exec();
	}
}
