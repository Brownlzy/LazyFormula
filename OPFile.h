#pragma once

#include <QObject>
#include <QApplication>
#include <QFileInfo>
#include <iostream>
#include <fstream>
#include "appinfo.h"
#include <QDebug>
#include "qaesencryption.h"
#include <QDateTime>
#include <QTextStream>
#include <QElapsedTimer>
using namespace std;

#define KEY_MAX_LENGTH 100
struct Formula
{
	QString name = "";
	double amount = 0;
	QString company = "";
	friend ostream& operator<<(ostream& out, const Formula& obj)
	{
		out << obj.name.toStdString() << "," << obj.amount << "," << obj.company.toStdString() << ",\n";
		return out;
	}
	friend istream& operator>>(istream& in, Formula& obj)
	{
		string tmp;
		in >> tmp;
		QString qtmp = QString::fromStdString(tmp);
		obj.name = qtmp.split(",")[0];
		obj.amount = qtmp.split(",")[1].toDouble();
		obj.company = qtmp.split(",")[2];
		return in;
	}
};

class BXORCryptUtil
{
public:
	BXORCryptUtil(QString key)
	{
		QTextStream ts(&key32);
		ts.setFieldWidth(32);
		ts.setPadChar('0');
		ts << key;
	}

	QString encrypt(QString text)
	{
		text += "#end";
		QByteArray line = text.toLocal8Bit();
		QElapsedTimer timer;
		timer.start();
		QByteArray qbr = QAESEncryption::Crypt(QAESEncryption::AES_256, QAESEncryption::ECB,
			line, key32.toLatin1());
		qDebug() << "The Qt-Class [slow operation] took" << timer.nsecsElapsed() << "nanoseconds";
		return QString::fromLocal8Bit(qbr.toBase64());
	}

	QString decrypt(QString text)
	{
		if (text == "") return text;
		QByteArray line = QByteArray::fromBase64(text.toLocal8Bit());
		QElapsedTimer timer;
		timer.start();
		QByteArray qbr = QAESEncryption::Decrypt(QAESEncryption::AES_256, QAESEncryption::ECB,
			line, key32.toLatin1());
		qDebug() << "The Qt-Class [slow operation] took" << timer.nsecsElapsed() << "nanoseconds";
		return QString::fromLocal8Bit(qbr).split("#end")[0];
	}

	QString encrypt(Formula* f)
	{
		QString text = f->name + "," + QString::number(f->amount) + "," + f->company + ",";
		QString cipher = encrypt(text);
		return cipher;
	}

	QString decrypt(QString text, Formula* f)
	{
		QString result = decrypt(text);
		f->name = result.split(",")[0];
		f->amount = result.split(",")[1].toDouble();
		f->company = result.split(",")[2];
		return result;
	}

private:
	QString key32 = "";
};



struct Index
{
	QString FName = "";
	QString Path = "";
	bool isCompleted = false;
	friend ostream& operator<<(ostream& out, const Index& obj)
	{
		out << obj.FName.toStdString() << "," << obj.Path.toStdString() << "," << obj.isCompleted << ",\n";
		return out;
	}
	friend istream& operator>>(istream& in, Index& obj)
	{
		string tmp;
		in >> tmp;
		QString qtmp = QString::fromStdString(tmp);
		obj.FName = qtmp.split(",")[0];
		obj.Path = qtmp.split(",")[1];
		if (qtmp.count(",") == 3)
			obj.isCompleted = qtmp.split(",")[2].toInt();
		else
			obj.isCompleted = 1;
		return in;
	}
};


class OPFile : public QObject
{
	Q_OBJECT
public:
	QString INIPath = "%AppDir%/config.ini";
	int numIndex = 0;
	Index* index = nullptr;
	int numFormula = 0;
	Formula* formula = nullptr;
	double allDosage = 0;
	int allMaterial = 0;
	QString creatTime = "";
	int verFormula = VERSIONID;
	BXORCryptUtil bc;
	OPFile(QObject* parent, QString mINIPath = "");
	~OPFile();
	int WriteMyINI();
	int ReadMyINI();
	int ReadIndex();
	int WriteIndex();
	int ReadFormula(int i);
	int ReadFormula(QString name);
	int WriteFormula(int i, bool isDone);
	int WriteFormula(QString name);
	int DeleteFormula(QString name);

	struct INI
	{
		int VersionID = VERSIONID;
		string Version = VERSION;
		bool isLogin = false;
		string IndexPath = "%DataDir%";
		string UserName = "Admin";
		string Password = "";
		string QuestionID = "0";
		string QAnswer = "root";
		string Material = "";
	}myini;

};
