#include "LoginForm.h"

int LoginForm::excode = 0;
LoginForm::LoginForm(QWidget* parent, QString pw)
	: QDialog(parent), Password(pw)
{
	ui.setupUi(this);
	SwitchToLogin();
	connect(ui.btnLogin, SIGNAL(clicked()), this, SLOT(Login()));
	connect(ui.btnBack, SIGNAL(clicked()), this, SLOT(SwitchToLogin()));
	connect(ui.btnForgetPW, SIGNAL(clicked()), this, SLOT(SwitchToFPW()));
	connect(ui.checkBox, SIGNAL(clicked()), this, SLOT(chkShowPW()));

}

LoginForm::~LoginForm()
{
}

void LoginForm::SwitchToLogin()
{
	ui.frmForgetPW->hide();
	ui.frmLogin->move(10, 10);
	ui.frmLogin->show();
	this->setFixedSize(ui.frmLogin->width() + 20, ui.frmLogin->height() + 20);
}

void LoginForm::SwitchToFPW()
{
	ui.frmLogin->hide();
	ui.frmForgetPW->move(10, 10);
	ui.frmForgetPW->show();
	this->setFixedSize(ui.frmForgetPW->width() + 20, ui.frmForgetPW->height() + 20);
}

void LoginForm::chkShowPW()
{
	if (ui.checkBox->checkState() == 0)
		ui.lineEdit->setEchoMode(QLineEdit::Password);
	else
		ui.lineEdit->setEchoMode(QLineEdit::Normal);

}

void LoginForm::Login()
{
	if (ui.lineEdit->text() == Password)
	{
		excode = 1;
		this->close();
	}
	else
	{
		excode = 0;
		ui.lineEdit->clear();
	}
}
