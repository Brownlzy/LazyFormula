#pragma once

#include <QtWidgets/QMainWindow>
#include <QApplication>
#include "ui_LazyFormula.h"
#include "Settings.h"
#include "OPFile.h"
#include <QTranslator>
#include "appinfo.h"
#include "LoginForm.h"
#include <QMessagebox>

class LazyFormula : public QMainWindow
{
	Q_OBJECT

public:
	LazyFormula(QWidget* parent = Q_NULLPTR);

private:
	Ui::LazyFormulaClass ui;
	Settings* pSettings;
	OPFile* pOPFile;
	int Initiallize();

public slots:
	void ShowSettings();
	void RefreshSettings();
	void RefreshIndex();
	void RefreshFormula();
	void RefreshFormula(QListWidgetItem* item);
	void SaveFormula();
	void AddIndex();
	void DeleteIndex();
	void resizeEvent(QResizeEvent* event);
	void PrepareEdit();
	void EditChanged(int x, int y);
	void btnE();
	void btnD();
};
