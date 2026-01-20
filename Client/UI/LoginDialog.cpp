#include "LoginDialog.h"
#include "ui_LoginDialog.h"
#include <QMessageBox>
#include <QRegularExpression>

LoginDialog::LoginDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::onLoginButtonClicked()
{
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();

    if (!validateInput()) {
        return;
    }

    emit loginSuccess(username, 0);
}

void LoginDialog::onRegisterButtonClicked()
{
    QMessageBox::information(this, "提示", "注册功能待实现");
}

void LoginDialog::validateInput()
{
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();

    if (username.isEmpty()) {
        QMessageBox::warning(this, "警告", "用户名不能为空");
        ui->usernameLineEdit->setFocus();
        return false;
    }

    if (password.isEmpty()) {
        QMessageBox::warning(this, "警告", "密码不能为空");
        ui->passwordLineEdit->setFocus();
        return false;
    }

    QRegularExpression usernameRegex("^[a-zA-Z0-9_]{3,20}$");
    if (!usernameRegex.match(username).hasMatch()) {
        QMessageBox::warning(this, "警告", "用户名格式不正确（3-20位字母、数字或下划线）");
        ui->usernameLineEdit->setFocus();
        return false;
    }

    if (password.length() < 6) {
        QMessageBox::warning(this, "警告", "密码长度不能少于6位");
        ui->passwordLineEdit->setFocus();
        return false;
    }

    return true;
}
