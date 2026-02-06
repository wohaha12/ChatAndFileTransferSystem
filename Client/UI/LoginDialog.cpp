#include "LoginDialog.h"
#include "ui_LoginDialog.h"
#include <QMessageBox>
#include <QRegularExpression>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::LoginDialog) {
  ui->setupUi(this);
  closeButton = ui->closeButton;
}

LoginDialog::~LoginDialog() { delete ui; }

void LoginDialog::setupDialog() {
  setWindowModality(Qt::ApplicationModal);
  setMinimumSize(420, 500);
  setMaximumSize(420, 500);

  connect(ui->loginButton, &QPushButton::clicked, this,
          &LoginDialog::onLoginButtonClicked);
  connect(ui->registerButton, &QPushButton::clicked, this,
          &LoginDialog::onRegisterButtonClicked);
  connect(ui->closeButton, &QPushButton::clicked, this,
          &LoginDialog::onCloseButtonClicked);
  // connect(ui->togglePasswordButton, &QPushButton::clicked, this,
  // &LoginDialog::onTogglePasswordClicked); connect(ui->clearUsernameButton,
  // &QPushButton::clicked, this, &LoginDialog::onClearUsernameClicked);
  // connect(ui->clearPasswordButton, &QPushButton::clicked, this,
  // &LoginDialog::onClearPasswordClicked);

  setupPasswordToggle();
}

void LoginDialog::paintEvent(QPaintEvent *event) { QDialog::paintEvent(event); }

// 移除setupClearButtons和相关函数，使用QLineEdit的actionButton功能

// 移除onClearUsernameClicked和onClearPasswordClicked，使用QLineEdit的actionButton功能

void LoginDialog::setupPasswordToggle() { togglePasswordVisibility(); }

void LoginDialog::togglePasswordVisibility() {
  /*
  if (ui->passwordLineEdit->echoMode() == QLineEdit::Normal) {
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
    ui->togglePasswordButton->setText("👁");
  } else {
    ui->passwordLineEdit->setEchoMode(QLineEdit::Normal);
    ui->togglePasswordButton->setText("🙈");
  }
  */
}

void LoginDialog::onTogglePasswordClicked() { togglePasswordVisibility(); }

void LoginDialog::onCloseButtonClicked() { QApplication::quit(); }

void LoginDialog::onLoginButtonClicked() {
  QString username = ui->usernameLineEdit->text().trimmed();
  QString password = ui->passwordLineEdit->text();

  if (!validateInput()) {
    return;
  }

  emit loginSuccess(username, 0);
}

void LoginDialog::onRegisterButtonClicked() {
  QMessageBox::information(this, "提示", "注册功能待实现");
}

bool LoginDialog::validateInput() {
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
    QMessageBox::warning(this, "警告",
                         "用户名格式不正确（3-20位字母、数字或下划线）");
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
