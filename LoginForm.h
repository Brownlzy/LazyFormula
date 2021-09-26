#pragma once

#include <QDialog>
#include <QLineEdit>
#include "ui_LoginForm.h"
#include "OPFile.h"

class LoginForm : public QDialog
{
	Q_OBJECT

public:
	LoginForm(QWidget* parent = Q_NULLPTR, QString pw = VERSION);
	~LoginForm();
	static int excode;
private:
	Ui::LoginForm ui;
	QString Password = VERSION;
private slots:
	void SwitchToLogin();
	void SwitchToFPW();
	void chkShowPW();
	void Login();
};
