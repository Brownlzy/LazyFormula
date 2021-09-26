#pragma once

#include <QDialog>
#include "ui_Settings.h"
#include "OPFile.h"
#include "update.h"

class Settings : public QDialog
{
	Q_OBJECT

public:
	OPFile* pOPFile;
	Settings(OPFile* p, QWidget* parent = Q_NULLPTR);
	~Settings();
public slots:
	int SaveSettings();
	void closeEvent(QCloseEvent* event);
	void btnDoUpdate();
private:
	Ui::Settings ui;
	int Initialize();
	Update* pUpdate;
};
