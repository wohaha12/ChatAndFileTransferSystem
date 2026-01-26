#ifndef CLIENT_UI_CHATWIDGET_H
#define CLIENT_UI_CHATWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>

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
    void adjustForWindowSize(int width, int height);

private slots:
    void onSendButtonClicked();
    void onMessageTextChanged();
    void onFriendClicked(QListWidgetItem* item);
    void onAddFriendClicked();
    void onMoreOptionsClicked();
    void onEmojiButtonClicked();

private:
    void setupUi();
    void applyStyles();
    void connectSignals();
    void addSampleFriends();

    Ui::ChatWidget* ui;
};

#endif // CLIENT_UI_CHATWIDGET_H
