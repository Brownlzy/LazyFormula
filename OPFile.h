#pragma once

#include <QObject>
#include <QApplication>
#include <QFileInfo>
#include <iostream>
#include <fstream>
#include "appinfo.h"
using namespace std;

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
	int verFormula = VERSIONID;
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
		string Password = "cm9vdA==";
		string QuestionID = "0";
		string QAnswer = "root";
		string Material = "";
	}myini;

};

extern string Encode(const unsigned char* Data, int DataByte);
extern string Decode(const char* Data, int DataByte, int& OutByte);
