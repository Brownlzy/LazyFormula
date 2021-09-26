#include "OPFile.h"

OPFile::OPFile(QObject* parent, QString mINIPath)
	: QObject(parent)
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
	fout << "Password = " << myini.Password << endl;
	fout << "QuestionID = " << myini.QuestionID << endl;
	fout << "QAnswer = " << myini.QAnswer << endl;
	fout << "Material = " << myini.Material << endl;
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
	fin >> verFormula >> numFormula >> allMaterial >> allDosage;
	if (numFormula != 0)
	{
		formula = new Formula[numFormula];
		for (int i = 0; i < numFormula; i++)
		{
			fin >> formula[i];
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

int OPFile::WriteFormula(int i)
{
	ofstream fout(myini.IndexPath + "/" + index[i].Path.toStdString());
	if (!fout)
	{
		return -1;
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
	fout << VERSIONID << " " << numFormula << " " << allMaterial << " " << allDosage << "\n";
	for (int i = 0; i < numFormula; i++)
	{
		fout << formula[i];
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
	QByteArray ba = name.toLatin1();
	unsigned char* str1 = (unsigned char*)ba.data();
	ofstream fout(myini.IndexPath + "/" + Encode(str1, ba.length()) + ".lfd");
	if (!fout)
	{
		return -2;
	}
	int numMaterial = QString::fromStdString(myini.Material).count("<br/>");
	numFormula = numMaterial + 1;
	allMaterial = 0;
	allDosage = 0;
	delete[] formula;
	formula = new Formula[numFormula];
	for (int i = 0; i < numFormula; i++)
	{
		formula[i].name = QString::fromStdString(myini.Material).split("<br/>")[i];
		formula[i].amount = 0;
		formula[i].company = "";
	}
	fout << VERSIONID << " " << numFormula << " " << allMaterial << " " << allDosage << "\n";
	for (int i = 0; i < numFormula; i++)
	{
		fout << formula[i];
	}
	fout.close();
	Index* tmp = new Index[++numIndex];
	tmp[numIndex - 1].FName = name;
	tmp[numIndex - 1].Path = QString::fromStdString(Encode(str1, ba.length()) + ".lfd");
	for (int i = 0; i < numIndex - 1; i++)
	{
		tmp[i].FName = index[i].FName;
		tmp[i].Path = index[i].Path;
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
