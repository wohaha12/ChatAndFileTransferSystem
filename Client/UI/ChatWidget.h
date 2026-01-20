#ifndef CLIENT_UI_CHATWIDGET_H
#define CLIENT_UI_CHATWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>

namespace Ui {
class ChatWidget;
}

class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWidget(QWidget* parent = nullptr);
    ~ChatWidget();

    void addMessage(const QString& sender, const QString& message, bool isSelf);

private slots:
    void onSendButtonClicked();
    void onMessageTextChanged();

private:
    Ui::ChatWidget* ui;
};

#endif // CLIENT_UI_CHATWIDGET_H
