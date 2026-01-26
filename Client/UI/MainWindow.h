#ifndef CLIENT_UI_MAINWINDOW_H
#define CLIENT_UI_MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QCloseEvent>
#include <QResizeEvent>

class LoginDialog;
class ChatWidget;
class FileWidget;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onLoginSuccess(const QString& username, uint64_t userId);
    void onLogout();
    void showChatWidget();
    void showFileWidget();

private:
    void setupUi();
    void applyStyles();
    void connectSignals();
    void adjustLayoutForSize();

    Ui::MainWindow* ui;
    LoginDialog* m_loginDialog;
    ChatWidget* m_chatWidget;
    FileWidget* m_fileWidget;
};

#endif // CLIENT_UI_MAINWINDOW_H
