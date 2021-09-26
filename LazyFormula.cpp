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
	connect(ui.tableEdit, SIGNAL(cellChanged(int, int)), this, SLOT(EditChanged(int, int)));
	connect(ui.txtNewFormula, SIGNAL(returnPressed()), this, SLOT(AddIndex()));
	return 0;
}

void LazyFormula::RefreshSettings()
{
	ui.labIndexPath->setText(QString(tr("Index is saved in:")) + QString::fromStdString(pOPFile->myini.IndexPath));
	ui.labIndexPath->setToolTip(QString(tr("Index is saved in:")) + QString::fromStdString(pOPFile->myini.IndexPath));
	int row = ui.listIndex->currentIndex().row();
	if (row != -1)
	{
		disconnect(ui.tableEdit, SIGNAL(cellChanged(int, int)), this, SLOT(EditChanged(int, int)));
		PrepareEdit();
		connect(ui.tableEdit, SIGNAL(cellChanged(int, int)), this, SLOT(EditChanged(int, int)));
	}
}

void LazyFormula::RefreshIndex()
{
	ui.listIndex->clear();
	ui.labF->setText(tr("numFormula:") + QString::number(pOPFile->numIndex));
	for (int i = 0; i < pOPFile->numIndex; i++)
	{
		ui.listIndex->addItem(new QListWidgetItem((pOPFile->index[i].isCompleted ? "" : "*") + pOPFile->index[i].FName));
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
		disconnect(ui.tableEdit, SIGNAL(cellChanged(int, int)), this, SLOT(EditChanged(int, int)));
		PrepareEdit();
		connect(ui.tableEdit, SIGNAL(cellChanged(int, int)), this, SLOT(EditChanged(int, int)));
	}
	else
	{
		ui.tableFormula->clearContents();
		ui.tableFormula->setRowCount(0);
		disconnect(ui.tableEdit, SIGNAL(cellChanged(int, int)), this, SLOT(EditChanged(int, int)));
		ui.tableEdit->clearContents();
		ui.tableEdit->setRowCount(0);
		connect(ui.tableEdit, SIGNAL(cellChanged(int, int)), this, SLOT(EditChanged(int, int)));
		ui.labD->setText(tr(":All Dosage"));
		ui.labM->setText(tr("All Material:"));
	}
}

void LazyFormula::RefreshFormula(QListWidgetItem* item)
{
	if (item->text().startsWith("*"))
		pOPFile->ReadFormula(item->text().split("*")[1]);
	else
		pOPFile->ReadFormula(item->text());
	ui.checkBox->setChecked(!item->text().startsWith("*"));
	RefreshFormula();
}

void LazyFormula::SaveFormula()
{
	int row = ui.listIndex->currentIndex().row();
	if (row != -1)
	{
		pOPFile->numFormula = ui.tableFormula->rowCount();
		if (ui.listIndex->item(row)->text().startsWith("*") && ui.checkBox->isChecked())
		{
			ui.listIndex->item(row)->setText(ui.listIndex->item(row)->text().split("*")[1]);
		}
		else if (!ui.listIndex->item(row)->text().startsWith("*") && !ui.checkBox->isChecked())
		{
			ui.listIndex->item(row)->setText("*" + ui.listIndex->item(row)->text());
		}
		Formula* tmp = new Formula[pOPFile->numFormula];
		delete[] pOPFile->formula;
		pOPFile->formula = tmp;
		for (int i = 0; i < pOPFile->numFormula; i++)
		{
			pOPFile->formula[i].name = ui.tableFormula->item(i, 0)->text();
			pOPFile->formula[i].amount = ui.tableFormula->item(i, 1)->text().toDouble();
			pOPFile->formula[i].company = ui.tableFormula->item(i, 2)->text();
		}
		for (int i = 0; i < pOPFile->numIndex; i++)
			if (ui.listIndex->currentItem()->text() == pOPFile->index[i].FName
				|| ui.listIndex->currentItem()->text().startsWith("*") && ui.listIndex->currentItem()->text().split("*")[1] == pOPFile->index[i].FName)
			{
				pOPFile->WriteFormula(i, ui.checkBox->isChecked());
			}
		RefreshFormula();
		ui.stBar->showMessage(tr("Susseffully Saved formula \"") + (ui.listIndex->item(row)->text().startsWith("*") ? ui.listIndex->item(row)->text().split("*")[1] : ui.listIndex->item(row)->text()) + tr("\"."));
	}
	else
	{
		ui.stBar->showMessage(tr("Please select an item."));
	}
}

void LazyFormula::AddIndex()
{
	if (ui.txtNewFormula->text().contains("*"))
	{
		ui.stBar->showMessage(tr("\"*\" cant be a part of formula name, Please change a name."));
		return;
	}
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
		QMessageBox::StandardButton result = QMessageBox::warning(NULL, tr("Warning"), tr("About to delete formula \"") + ui.listIndex->currentItem()->text() + tr("\"\nPress \"No\" to Cancel."), QMessageBox::Yes | QMessageBox::No);
		switch (result)
		{
		case QMessageBox::Yes:
			if (ui.listIndex->currentItem()->text().startsWith("*"))
				pOPFile->DeleteFormula(ui.listIndex->currentItem()->text().split("*")[1]);
			else
				pOPFile->DeleteFormula(ui.listIndex->currentItem()->text());
			RefreshIndex();
			RefreshFormula();
			ui.tableFormula->clearContents();
			ui.tableFormula->setRowCount(0);
			ui.labD->setText("0" + tr(":All Dosage"));
			ui.labM->setText(tr("All Material:") + "0");
			ui.stBar->showMessage(tr("Deleted Successfully"));
			break;
		case QMessageBox::No:
			return;
			break;
		default:
			break;
		}
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
	ui.tableFormula->setGeometry(ui.tableFormula->x(), ui.tableFormula->y(), ui.tableFormula->width(), y - 120);
	ui.tableEdit->setGeometry(ui.tableEdit->x(), ui.tableEdit->y(), ui.tableEdit->width(), y - 120);
	//if (x < 672)ui.labD->move(x - 371, ui.labD->y());
}

void LazyFormula::PrepareEdit()
{
	ui.tableEdit->clearContents();
	int numMaterial = QString::fromStdString(pOPFile->myini.Material).count("<br/>") + 1;
	ui.tableEdit->setRowCount(numMaterial);
	for (int i = 0; i < numMaterial; i++)
	{
		ui.tableEdit->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(pOPFile->myini.Material).split("<br/>")[i]));
		ui.tableEdit->setItem(i, 1, new QTableWidgetItem("0"));
		ui.tableEdit->setItem(i, 2, new QTableWidgetItem("-"));
	}
	int flag = 1;
	for (int i = 0; i < ui.tableFormula->rowCount(); i++)
	{
		for (int j = 0; j < ui.tableEdit->rowCount(); j++)
		{
			if (ui.tableEdit->item(j, 0)->text() == ui.tableFormula->item(i, 0)->text())
			{
				ui.tableEdit->item(j, 1)->setText(ui.tableFormula->item(i, 1)->text());
				ui.tableEdit->item(j, 2)->setText(ui.tableFormula->item(i, 2)->text());
				flag = 0;
				break;
			}
		}
		if (flag)
		{
			ui.tableEdit->setRowCount(ui.tableEdit->rowCount() + 1);
			ui.tableEdit->setItem(ui.tableEdit->rowCount() - 1, 0, new QTableWidgetItem(ui.tableFormula->item(i, 0)->text()));
			ui.tableEdit->setItem(ui.tableEdit->rowCount() - 1, 1, new QTableWidgetItem(ui.tableFormula->item(i, 1)->text()));
			ui.tableEdit->setItem(ui.tableEdit->rowCount() - 1, 2, new QTableWidgetItem(ui.tableFormula->item(i, 2)->text()));
		}
	}
}

void LazyFormula::EditChanged(int x, int y)
{
	if (ui.tableEdit->item(x, 1)->text() == "" || ui.tableEdit->item(x, 1)->text() == "0")
	{
		for (int i = 0; i < ui.tableFormula->rowCount(); i++)
		{
			if (ui.tableFormula->item(i, 0)->text() == ui.tableEdit->item(x, 0)->text())
			{
				ui.tableFormula->removeRow(i);
				break;
			}
		}
	}
	else
	{
		int flag = 1;
		for (int i = 0; i < ui.tableFormula->rowCount(); i++)
		{
			if (ui.tableFormula->item(i, 0)->text() == ui.tableEdit->item(x, 0)->text())
			{
				ui.tableFormula->item(i, 1)->setText(ui.tableEdit->item(x, 1)->text());
				ui.tableFormula->item(i, 2)->setText(ui.tableEdit->item(x, 2)->text());
				flag = 0;
				break;
			}
		}
		if (flag)
		{
			ui.tableFormula->setRowCount(ui.tableFormula->rowCount() + 1);
			ui.tableFormula->setItem(ui.tableFormula->rowCount() - 1, 0, new QTableWidgetItem(ui.tableEdit->item(x, 0)->text()));
			ui.tableFormula->setItem(ui.tableFormula->rowCount() - 1, 1, new QTableWidgetItem(ui.tableEdit->item(x, 1)->text()));
			ui.tableFormula->setItem(ui.tableFormula->rowCount() - 1, 2, new QTableWidgetItem(ui.tableEdit->item(x, 2)->text()));
		}
	}
}

void LazyFormula::ShowSettings()
{
	pSettings = new Settings(pOPFile);
	pSettings->setModal(this);
	pSettings->show();
	pSettings->exec();
	RefreshSettings();
}
