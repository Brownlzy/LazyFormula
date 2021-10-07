#include "OPFile.h"

OPFile::OPFile(QObject* parent, QString mINIPath)
	: QObject(parent),
	bc(QString::fromStdString(VERSION) + QString::number(VERSIONID))
{
	if (mINIPath == "")
	{
		INIPath = INIPath.replace("%AppDir%", qApp->applicationDirPath());
	}
}

OPFile::~OPFile()
{
}

int OPFile::WriteMyINI()
{
	ofstream fout(INIPath.toStdString());
	if (!fout)
	{
		return 1;
	}
	fout << "VersionID = " << VERSIONID << endl;
	fout << "Version = " << VERSION << endl;
	fout << "isLogin = " << myini.isLogin << endl;
	fout << "IndexPath = " << myini.IndexPath << endl;
	fout << "UserName = " << myini.UserName << endl;
	fout << "Password = " << bc.encrypt(QString::fromStdString(myini.Password)).toStdString() << endl;
	fout << "QuestionID = " << myini.QuestionID << endl;
	fout << "QAnswer = " << myini.QAnswer << endl;
	//QByteArray ba = QString::fromStdString(myini.Material).toLocal8Bit().toBase64();
	//fout << "Material = " << QString::fromLocal8Bit(ba).toStdString() << endl;
	fout << "Material = " << bc.encrypt(QString::fromStdString(myini.Material)).toStdString() << endl;
	fout.close();
	return 0;
}

int OPFile::ReadMyINI()
{
	string str_tmp;
	QFileInfo file(INIPath);
	if (file.exists() == false)
		WriteMyINI();
	ifstream fin(INIPath.toStdString());
	if (!fin)
	{
		return 1;
	}
	else
	{
		for (; !fin.eof(); fin >> str_tmp)
		{
			if ("VersionID" == str_tmp)
				fin >> str_tmp >> myini.VersionID;
			else if ("Version" == str_tmp)
				fin >> str_tmp >> myini.Version;
			else if ("isLogin" == str_tmp)
				fin >> str_tmp >> myini.isLogin;
			else if ("IndexPath" == str_tmp)
			{
				fin >> str_tmp >> myini.IndexPath;
			}
			else if ("UserName" == str_tmp)
				fin >> str_tmp >> myini.UserName;
			else if ("Password" == str_tmp)
				fin >> str_tmp >> myini.Password;
			else if ("QuestionID" == str_tmp)
				fin >> str_tmp >> myini.QuestionID;
			else if ("QAnswer" == str_tmp)
				fin >> str_tmp >> myini.QAnswer;
			else if ("Material" == str_tmp)
				fin >> str_tmp >> myini.Material;
		}
		if (myini.VersionID >= 600)
		{
			BXORCryptUtil bctmp(QString::fromStdString(myini.Version) + QString::number(myini.VersionID));
			myini.Material = bctmp.decrypt(QString::fromStdString(myini.Material)).toStdString();
			myini.Password = bctmp.decrypt(QString::fromStdString(myini.Password)).toStdString();
			//QByteArray ba = QByteArray::fromBase64(QString::fromStdString(myini.Material).toLocal8Bit());
			//myini.Material = QString::fromLocal8Bit(ba).toStdString();
		}
		else
		{
			QByteArray ptmp = QByteArray::fromBase64(QByteArray::fromStdString(myini.Password));
			myini.Password = ptmp.toStdString();
		}
	}
	fin.close();
	return 0;
}

int OPFile::ReadIndex()
{
	if (myini.IndexPath.compare("DataDir%") == 0)
	{
		myini.IndexPath = QString::fromStdString(myini.IndexPath).replace("%DataDir%", qApp->applicationDirPath() + "/Data").toStdString();
	}
	QFileInfo file(QString::fromStdString(myini.IndexPath + "/index.lfi"));
	if (file.exists() == false)
		WriteIndex();
	ifstream fin(myini.IndexPath + "/index.lfi");
	if (!fin)
	{
		return -1;
	}
	fin >> numIndex;
	if (numIndex != 0)
	{
		index = new Index[numIndex];
		for (int i = 0; i < numIndex; i++)
		{
			fin >> index[i];
		}
	}
	fin.close();
	return numIndex;
}

int OPFile::WriteIndex()
{
	ofstream fout(myini.IndexPath + "/index.lfi");
	if (!fout)
	{
		return -1;
	}
	fout << numIndex << "\n";
	for (int i = 0; i < numIndex; i++)
	{
		fout << index[i];
	}
	fout.close();
	return 0;
}

int OPFile::ReadFormula(int i)
{
	QFileInfo file(QString::fromStdString(myini.IndexPath) + "/" + index[i].Path);
	if (file.exists() == false)
		return -1;
	ifstream fin(myini.IndexPath + "/" + index[i].Path.toStdString());
	if (!fin)
	{
		return -2;
	}
	string tmpTime;
	fin >> verFormula;
	QString key = "0\n0\n0\n0\n0\n0";
	if (verFormula >= 601)
	{
		fin >> numFormula >> allMaterial >> allDosage >> tmpTime;
		creatTime = QString::fromStdString(tmpTime);
	}
	else
	{
		fin >> numFormula >> allMaterial >> allDosage;
	}
	BXORCryptUtil tmpbc(creatTime);
	if (numFormula != 0)
	{
		formula = new Formula[numFormula];
		for (int i = 0; i < numFormula; i++)
		{
			if (verFormula >= 600)
			{
				fin >> tmpTime;
				tmpbc.decrypt(QString::fromStdString(tmpTime), &formula[i]);
			}
			else
			{
				fin >> formula[i];
			}
		}
	}
	fin.close();
	return 0;
}

int OPFile::ReadFormula(QString name)
{
	for (int i = 0; i < numIndex; i++)
	{
		if (name == index[i].FName)
			return ReadFormula(i);
	}
	return -1;
}

int OPFile::WriteFormula(int i, bool isDone)
{
	ofstream fout(myini.IndexPath + "/" + index[i].Path.toStdString());
	if (!fout)
	{
		return -1;
	}
	if (isDone != index[i].isCompleted)
	{
		index[i].isCompleted = isDone;
		WriteIndex();
	}
	allMaterial = 0;
	allDosage = 0;
	for (int i = 0; i < numFormula; i++)
	{
		if (formula[i].amount)
		{
			allMaterial++;
			allDosage += formula[i].amount;
		}
	}
	if (verFormula < 600)
	{
		QDateTime curDateTime = QDateTime::currentDateTime();
		creatTime = curDateTime.toString("yyyyMMddhhmmss");
	}
	fout << VERSIONID << " " << numFormula << " " << allMaterial << " " << allDosage << " " << creatTime.toStdString() << "\n";
	verFormula = VERSIONID;
	BXORCryptUtil tmpbc(creatTime);
	for (int i = 0; i < numFormula; i++)
	{
		fout << tmpbc.encrypt(&formula[i]).toStdString() << "\n";
	}
	fout.close();
	return 0;
}

int OPFile::WriteFormula(QString name)
{
	for (int i = 0; i < numIndex; i++)
	{
		if (name == index[i].FName)
			return -1;
	}
	QByteArray ba = name.toLocal8Bit().toBase64();
	ofstream fout(myini.IndexPath + "/" + QString::fromLocal8Bit(ba).replace("/", "-").toStdString() + ".lfd");
	if (!fout)
	{
		return -2;
	}
	//int numMaterial = QString::fromStdString(myini.Material).count("<br/>");
	//numFormula = numMaterial + 1;
	numFormula = 0;
	allMaterial = 0;
	allDosage = 0;
	QDateTime curDateTime = QDateTime::currentDateTime();
	creatTime = curDateTime.toString("yyyyMMddhhmmss");
	delete[] formula;
	formula = new Formula[1];
	//for (int i = 0; i < numFormula; i++)
	//{
	formula[0].name = "";//QString::fromStdString(myini.Material).split("<br/>")[i];
	formula[0].amount = 0;
	formula[0].company = "";
	//}
	fout << VERSIONID << " " << numFormula << " " << allMaterial << " " << allDosage << " " << creatTime.toStdString() << "\n";
	//for (int i = 0; i < numFormula; i++)
	//{
	BXORCryptUtil tmpbc(creatTime);
	fout << tmpbc.encrypt(&formula[0]).toStdString() << "\n";
	//}
	fout.close();
	Index* tmp = new Index[++numIndex];
	tmp[numIndex - 1].FName = name;
	tmp[numIndex - 1].Path = QString::fromLocal8Bit(ba).replace("/", "-") + ".lfd";
	tmp[numIndex - 1].isCompleted = false;
	for (int i = 0; i < numIndex - 1; i++)
	{
		tmp[i].FName = index[i].FName;
		tmp[i].Path = index[i].Path;
		tmp[i].isCompleted = index[i].isCompleted;
	}
	delete[] index;
	index = tmp;
	WriteIndex();
	return 0;
}

int OPFile::DeleteFormula(QString name)
{
	for (int i = 0; i < numIndex; i++)
	{
		if (index[i].FName == name)
		{
			numIndex--;
			QFile fileTemp(QString::fromStdString(myini.IndexPath + "/") + index[i].Path);
			fileTemp.remove();
			for (int j = i; j < numIndex; j++)
			{
				index[j].FName = index[j + 1].FName;
				index[j].Path = index[j + 1].Path;
			}
			index[numIndex].FName = "";
			index[numIndex].Path = "";
			WriteIndex();
			break;
		}
	}
	return 0;
}
