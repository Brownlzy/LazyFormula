#include "LazyFormula.h"

LazyFormula::LazyFormula(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	Initiallize();
}

int LazyFormula::Initiallize()
{
	this->setWindowTitle(QString("LazyFormula By:Brownlzy v") + VERSION + "(" + QString::number(VERSIONID) + ")");
	pOPFile = new OPFile(this, "");
	pOPFile->ReadMyINI();
	RefreshSettings();
	if (pOPFile->myini.isLogin)
	{
		this->hide();
		QByteArray ba = QString::fromStdString(pOPFile->myini.Password).toLatin1();
		char* str1 = (char*)ba.data();
		int a;
		LoginForm l(this, QString::fromStdString(Decode(str1, ba.length(), a)));
		l.setModal(this);
		l.show();
		l.exec();
		if (LoginForm::excode == 0)
		{
			QTimer::singleShot(0, qApp, &QCoreApplication::quit);
		}
	}
	connect(ui.btnSettings, SIGNAL(clicked()), this, SLOT(ShowSettings()));
	pOPFile->ReadIndex();
	RefreshIndex();
	connect(ui.btnNewFormula, SIGNAL(clicked()), this, SLOT(AddIndex()));
	connect(ui.btnDelete, SIGNAL(clicked()), this, SLOT(DeleteIndex()));
	connect(ui.listIndex, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(RefreshFormula(QListWidgetItem*)));
	connect(ui.listIndex, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(RefreshFormula(QListWidgetItem*)));
	connect(ui.btnReload, SIGNAL(clicked()), this, SLOT(RefreshFormula()));
	connect(ui.btnSave, SIGNAL(clicked()), this, SLOT(SaveFormula()));
	return 0;
}

void LazyFormula::RefreshSettings()
{
	ui.labIndexPath->setText(QString(tr("Index is saved in:")) + QString::fromStdString(pOPFile->myini.IndexPath));
}

void LazyFormula::RefreshIndex()
{
	ui.listIndex->clear();
	ui.labF->setText(tr("numFormula:") + QString::number(pOPFile->numIndex));
	for (int i = 0; i < pOPFile->numIndex; i++)
	{
		ui.listIndex->addItem(new QListWidgetItem(pOPFile->index[i].FName));
	}
}

void LazyFormula::RefreshFormula()
{
	int row = ui.listIndex->currentIndex().row();
	if (row != -1)
	{
		ui.labD->setText(QString::number(pOPFile->allDosage) + tr(":All Dosage"));
		ui.labM->setText(tr("All Material:") + QString::number(pOPFile->allMaterial));
		ui.tableFormula->clearContents();
		ui.tableFormula->setRowCount(pOPFile->numFormula);
		for (int i = 0; i < pOPFile->numFormula; i++)
		{
			ui.tableFormula->setItem(i, 0, new QTableWidgetItem(pOPFile->formula[i].name));
			ui.tableFormula->setItem(i, 1, new QTableWidgetItem(QString::number(pOPFile->formula[i].amount)));
			ui.tableFormula->setItem(i, 2, new QTableWidgetItem(pOPFile->formula[i].company));
			if (pOPFile->formula[i].amount > 0 && pOPFile->allDosage > 0) ui.tableFormula->setItem(i, 3, new QTableWidgetItem(QString::number((int)(pOPFile->formula[i].amount / pOPFile->allDosage * 100)) + "%"));
		}
	}
}

void LazyFormula::RefreshFormula(QListWidgetItem* item)
{
	pOPFile->ReadFormula(item->text());
	RefreshFormula();
}

void LazyFormula::SaveFormula()
{
	int row = ui.listIndex->currentIndex().row();
	if (row != -1)
	{
		pOPFile->numFormula = ui.tableFormula->rowCount();
		for (int i = 0; i < pOPFile->numFormula; i++)
		{
			pOPFile->formula[i].name = ui.tableFormula->item(i, 0)->text();
			pOPFile->formula[i].amount = ui.tableFormula->item(i, 1)->text().toDouble();
			pOPFile->formula[i].company = ui.tableFormula->item(i, 2)->text();
		}
		for (int i = 0; i < pOPFile->numIndex; i++)
			if (ui.listIndex->currentItem()->text() == pOPFile->index[i].FName)
				pOPFile->WriteFormula(i);
		RefreshFormula();
	}
	else
	{
		ui.stBar->showMessage(tr("Please select an item."));
	}
}

void LazyFormula::AddIndex()
{
	if (ui.txtNewFormula->text() != "")
		switch (pOPFile->WriteFormula(ui.txtNewFormula->text()))
		{
		case 0:
			ui.stBar->showMessage(tr("Successfully add Formula \"") + ui.txtNewFormula->text() + tr("\""));
			ui.txtNewFormula->clear();
			break;
		case -1:
			ui.stBar->showMessage(tr("There is already a \"") + ui.txtNewFormula->text() + tr("\", Please change a name"));
			break;
		case -2:
			ui.stBar->showMessage(tr("Failed to write file: \"index.lfi\"!."));
			break;
		}
	else
		ui.stBar->showMessage(tr("Please enter a name."));
	RefreshIndex();
}

void LazyFormula::DeleteIndex()
{
	int row = ui.listIndex->currentIndex().row();
	if (row != -1)
	{
		pOPFile->DeleteFormula(ui.listIndex->currentItem()->text());
		RefreshIndex();
		ui.tableFormula->clearContents();
		ui.tableFormula->setRowCount(0);
		ui.labD->setText("0" + tr(":All Dosage"));
		ui.labM->setText(tr("All Material:") + "0");
		ui.stBar->showMessage(tr("Deleted Successfully"));
	}
	else
	{
		ui.stBar->showMessage(tr("Please select an item."));
	}
}

void LazyFormula::resizeEvent(QResizeEvent* event)
{
	int x = this->frameGeometry().width();
	int y = this->frameGeometry().height();
	ui.listIndex->setGeometry(ui.listIndex->x(), ui.listIndex->y(), ui.listIndex->width(), y - 120);
	ui.tableFormula->setGeometry(ui.tableFormula->x(), ui.tableFormula->y(), x - 200, y - 120);
	if (x < 672)ui.labD->move(x - 371, ui.labD->y());
}

void LazyFormula::ShowSettings()
{
	pSettings = new Settings(pOPFile);
	pSettings->show();
	pSettings->exec();
	RefreshSettings();
}
