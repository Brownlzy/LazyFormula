#include "Settings.h"

Settings::Settings(OPFile* p, QWidget* parent)
	: pOPFile(p), QDialog(parent)
{
	ui.setupUi(this);
	Initialize();
}

Settings::~Settings()
{
}

void Settings::closeEvent(QCloseEvent* event)
{

}

int Settings::SaveSettings()
{
	if (ui.txtPassword->text().contains("#end") || ui.txtMaterial->toPlainText().contains("#end") || ui.txtAnswer->text().contains("#end"))
	{
		QMessageBox::warning(this, tr("Warning"), tr("\"#end\" isn't allowed in Password and Material and Answer."), QMessageBox::Ok);
		return 0;
	}
	pOPFile->myini.IndexPath = ui.txtIndexPath->text().replace(" ", "&nbsp").toStdString();
	pOPFile->myini.QAnswer = ui.txtAnswer->text().replace(" ", "").toStdString();
	pOPFile->myini.isLogin = ui.chkLogin->checkState();
	pOPFile->myini.Password = ui.txtPassword->text().toStdString();
	pOPFile->myini.Material = ui.txtMaterial->toPlainText().replace("\n", "<br/>").toStdString();
	//if (!ui.txtMaterial->toPlainText().endsWith("\n"))
	pOPFile->WriteMyINI();
	this->close();
	return 0;
}

int Settings::Initialize()
{
	ui.txtIndexPath->setText(QString::fromStdString(pOPFile->myini.IndexPath).replace("&nbsp", " "));
	ui.txtAnswer->setText(QString::fromStdString(pOPFile->myini.QAnswer));
	ui.chkLogin->setChecked(pOPFile->myini.isLogin);
	ui.txtPassword->setText(QString::fromStdString(pOPFile->myini.Password));
	ui.txtMaterial->setPlainText(QString::fromStdString(pOPFile->myini.Material).replace("<br/>", "\n"));
	connect(ui.btnSave, SIGNAL(clicked()), this, SLOT(SaveSettings()));
	pUpdate = new Update(this, ui.progressBar, ui.labUpdate, ui.txtUpdate);
	connect(ui.btnUpdate, SIGNAL(clicked()), this, SLOT(btnDoUpdate()));
	ui.labUpdate->setText(QString("Build Time: ") + RELEASETIME);
	ui.labVersion->setText(QString("v") + VERSION + "(" + QString::number(VERSIONID) + ")");
	this->setWindowIcon(QIcon(":/pic/s.ico"));
	return 0;
}

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
