#include "LoginDialog.h"
#include "ui_LoginDialog.h"
#include "StyleManager.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QTimer>
#include <QPainter>
#include <QLinearGradient>

LoginDialog::LoginDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    setupDialog();
    applyStyles();
    setupAnimations();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::setupDialog()
{
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowModality(Qt::ApplicationModal);
    setMinimumSize(420, 500);
    setMaximumSize(420, 500);
    
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginButtonClicked);
    connect(ui->registerButton, &QPushButton::clicked, this, &LoginDialog::onRegisterButtonClicked);
}

void LoginDialog::applyStyles()
{
    StyleManager* styleManager = StyleManager::instance();

    styleManager->applyLabelStyle(ui->titleLabel, true);
    styleManager->applyLabelStyle(ui->usernameLabel, false);
    styleManager->applyLabelStyle(ui->passwordLabel, false);
    styleManager->applyLineEditStyle(ui->usernameLineEdit);
    styleManager->applyLineEditStyle(ui->passwordLineEdit);
    styleManager->applyButtonStyle(ui->loginButton, true);
    styleManager->applyButtonStyle(ui->registerButton, false);

    QString logoStyle = QString(
        "QLabel {"
        "   color: %1;"
        "   background: transparent;"
        "}"
    ).arg(styleManager->primaryColor().name());
    ui->logoLabel->setStyleSheet(logoStyle);
}

void LoginDialog::setupAnimations()
{
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(opacityEffect);

    QPropertyAnimation* fadeInAnimation = new QPropertyAnimation(opacityEffect, "opacity", this);
    fadeInAnimation->setDuration(300);
    fadeInAnimation->setStartValue(0.0);
    fadeInAnimation->setEndValue(1.0);
    fadeInAnimation->start(QPropertyAnimation::DeleteWhenStopped);
}

void LoginDialog::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    StyleManager* styleManager = StyleManager::instance();
    QColor primaryColor = styleManager->primaryColor();
    QColor secondaryColor = styleManager->secondaryColor();

    QLinearGradient gradient(0, 0, width(), height());
    gradient.setColorAt(0.0, primaryColor);
    gradient.setColorAt(1.0, secondaryColor);

    painter.fillRect(rect(), gradient);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 255, 255, 240));
    painter.drawRoundedRect(10, 10, width() - 20, height() - 20, 15, 15);

    QDialog::paintEvent(event);
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

bool LoginDialog::validateInput()
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
