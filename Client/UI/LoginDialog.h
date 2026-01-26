#ifndef CLIENT_UI_LOGIN_DIALOG_H
#define CLIENT_UI_LOGIN_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QPaintEvent>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget* parent = nullptr);
    ~LoginDialog();

signals:
    void loginSuccess(const QString& username, uint64_t userId);
    void loginFailed(const QString& errorMsg);

private slots:
    void onLoginButtonClicked();
    void onRegisterButtonClicked();

private:
    void setupDialog();
    void applyStyles();
    void setupAnimations();
    bool validateInput();
    void paintEvent(QPaintEvent* event) override;

    Ui::LoginDialog* ui;
};

#endif // CLIENT_UI_LOGIN_DIALOG_H
