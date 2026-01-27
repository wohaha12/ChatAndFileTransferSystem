#include "ChatWidget.h"
#include "ui_ChatWidget.h"
#include "StyleManager.h"
#include "AnimationManager.h"
#include <QDateTime>
#include <QListWidgetItem>
#include <QScrollBar>

ChatWidget::ChatWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ChatWidget)
{
    ui->setupUi(this);
    setupUi();
    applyStyles();
    connectSignals();
}

ChatWidget::~ChatWidget()
{
    delete ui;
}

void ChatWidget::setupUi()
{
    addSampleFriends();
    
    connect(ui->sendButton, &QPushButton::clicked, this, &ChatWidget::onSendButtonClicked);
    connect(ui->messageLineEdit, &QLineEdit::textChanged, this, &ChatWidget::onMessageTextChanged);
    connect(ui->friendsList, &QListWidget::itemClicked, this, &ChatWidget::onFriendClicked);
    connect(ui->addFriendButton, &QPushButton::clicked, this, &ChatWidget::onAddFriendClicked);
    connect(ui->moreOptionsButton, &QPushButton::clicked, this, &ChatWidget::onMoreOptionsClicked);
    connect(ui->emojiButton, &QPushButton::clicked, this, &ChatWidget::onEmojiButtonClicked);
}

void ChatWidget::connectSignals()
{
    // 信号连接已在 setupUi() 中完成
}

void ChatWidget::applyStyles()
{
    StyleManager* styleManager = StyleManager::instance();

    QString friendsListStyle = QString(
        "QWidget#friendsListWidget {"
        "   background-color: %1;"
        "   border-right: 1px solid %2;"
        "}"
        "QWidget#friendsHeaderWidget {"
        "   background-color: %3;"
        "}"
        "QLabel#friendsTitleLabel {"
        "   color: %4;"
        "}"
        "QPushButton#addFriendButton {"
        "   background-color: %5;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 15px;"
        "}"
        "QPushButton#addFriendButton:hover {"
        "   background-color: %6;"
        "}"
        "QListWidget {"
        "   background-color: transparent;"
        "   border: none;"
        "}"
        "QListWidget::item {"
        "   padding: 12px 15px;"
        "   border-bottom: 1px solid %2;"
        "   color: %4;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: %7;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: %5;"
        "   color: white;"
        "}"
    ).arg(styleManager->cardBackgroundColor().name(),
           styleManager->borderColor().name(),
           styleManager->hoverColor().name(),
           styleManager->textColor().name(),
           styleManager->primaryColor().name(),
           "#1976D2",
           styleManager->hoverColor().name());
    ui->friendsListWidget->setStyleSheet(friendsListStyle);

    QString chatAreaStyle = QString(
        "QWidget#chatAreaWidget {"
        "   background-color: %1;"
        "}"
        "QWidget#chatHeaderWidget {"
        "   background-color: %2;"
        "   border-bottom: 1px solid %3;"
        "}"
        "QLabel#chatAvatarLabel {"
        "   background-color: %4;"
        "   border-radius: 15px;"
        "   border: 2px solid %4;"
        "}"
        "QLabel#chatNameLabel {"
        "   color: %5;"
        "}"
        "QPushButton#moreOptionsButton {"
        "   background-color: transparent;"
        "   border: none;"
        "   color: %5;"
        "   border-radius: 15px;"
        "}"
        "QPushButton#moreOptionsButton:hover {"
        "   background-color: %6;"
        "}"
        "QWidget#inputWidget {"
        "   background-color: %2;"
        "   border-top: 1px solid %3;"
        "}"
    ).arg(styleManager->backgroundColor().name(),
           styleManager->cardBackgroundColor().name(),
           styleManager->borderColor().name(),
           styleManager->primaryColor().name(),
           styleManager->textColor().name(),
           styleManager->hoverColor().name());
    ui->chatAreaWidget->setStyleSheet(chatAreaStyle);

    QString lineStyle = QString(
        "QFrame[frameShape=\"4\"], QFrame[frameShape=\"5\"] {"
        "   color: %1;"
        "   background-color: %1;"
        "}"
    ).arg(styleManager->borderColor().name());
    ui->friendsLine->setStyleSheet(lineStyle);
    ui->chatLine->setStyleSheet(lineStyle);
    ui->inputLine->setStyleSheet(lineStyle);

    styleManager->applyTextEditStyle(ui->chatTextEdit);
    styleManager->applyLineEditStyle(ui->messageLineEdit);
    styleManager->applyButtonStyle(ui->sendButton, true);

    QString emojiButtonStyle = QString(
        "QPushButton {"
        "   background-color: transparent;"
        "   border: none;"
        "   border-radius: 20px;"
        "}"
        "QPushButton:hover {"
        "   background-color: %1;"
        "}"
    ).arg(styleManager->hoverColor().name());
    ui->emojiButton->setStyleSheet(emojiButtonStyle);
}

void ChatWidget::addSampleFriends()
{
    QListWidgetItem* item1 = new QListWidgetItem("👤 张三");
    item1->setData(Qt::UserRole, "张三");
    ui->friendsList->addItem(item1);

    QListWidgetItem* item2 = new QListWidgetItem("👤 李四");
    item2->setData(Qt::UserRole, "李四");
    ui->friendsList->addItem(item2);

    QListWidgetItem* item3 = new QListWidgetItem("👤 王五");
    item3->setData(Qt::UserRole, "王五");
    ui->friendsList->addItem(item3);
}

void ChatWidget::addMessage(const QString& sender, const QString& message, bool isSelf)
{
    QString time = QDateTime::currentDateTime().toString("HH:mm:ss");
    QString html;

    StyleManager* styleManager = StyleManager::instance();

    if (isSelf) {
        html = QString("<div style='text-align: right; margin: 10px 20px;'>"
                       "<span style='color: %1; font-size: 12px;'>%2</span><br/>"
                       "<div style='display: inline-block; max-width: 60%; text-align: left;'>"
                       "<div style='background-color: %3; color: white; padding: 10px 15px; "
                       "border-radius: 15px 15px 0 15px; margin-top: 5px; word-wrap: break-word;'>%4</div>"
                       "</div></div>")
               .arg(styleManager->secondaryTextColor().name(), time, styleManager->primaryColor().name(), message);
    } else {
        html = QString("<div style='text-align: left; margin: 10px 20px;'>"
                       "<span style='color: %1; font-size: 12px;'>%2</span><br/>"
                       "<div style='display: inline-block; max-width: 60%; text-align: left;'>"
                       "<div style='background-color: %3; color: %4; padding: 10px 15px; "
                       "border-radius: 15px 15px 15px 0; margin-top: 5px; word-wrap: break-word;'>%5</div>"
                       "</div></div>")
               .arg(styleManager->secondaryTextColor().name(), time, 
                    styleManager->hoverColor().name(), styleManager->textColor().name(), message);
    }

    ui->chatTextEdit->append(html);
    ui->chatTextEdit->verticalScrollBar()->setValue(ui->chatTextEdit->verticalScrollBar()->maximum());
}

void ChatWidget::onSendButtonClicked()
{
    QString message = ui->messageLineEdit->text().trimmed();

    if (message.isEmpty()) {
        return;
    }

    addMessage("我", message, true);
    ui->messageLineEdit->clear();
}

void ChatWidget::onMessageTextChanged()
{
    ui->sendButton->setEnabled(!ui->messageLineEdit->text().trimmed().isEmpty());
}

void ChatWidget::onFriendClicked(QListWidgetItem* item)
{
    if (!item) {
        return;
    }

    QString friendName = item->data(Qt::UserRole).toString();
    ui->chatNameLabel->setText(friendName);
    
    AnimationManager::instance()->pulse(ui->chatNameLabel, 500);
    
    ui->chatTextEdit->clear();
    ui->chatTextEdit->append(QString("<div style='text-align: center; color: %1; padding: 20px;'>"
                                    "开始与 %2 聊天</div>")
                            .arg(StyleManager::instance()->secondaryTextColor().name(), friendName));
}

void ChatWidget::onAddFriendClicked()
{
    qDebug() << "Add friend button clicked";
}

void ChatWidget::onMoreOptionsClicked()
{
    qDebug() << "More options button clicked";
}

void ChatWidget::onEmojiButtonClicked()
{
    qDebug() << "Emoji button clicked";
}

void ChatWidget::adjustForWindowSize(int width, int height)
{
    if (width < 1000) {
        ui->friendsListWidget->setMaximumWidth(220);
        ui->friendsListWidget->setMinimumWidth(220);
    } else if (width < 1200) {
        ui->friendsListWidget->setMaximumWidth(250);
        ui->friendsListWidget->setMinimumWidth(250);
    } else {
        ui->friendsListWidget->setMaximumWidth(280);
        ui->friendsListWidget->setMinimumWidth(280);
    }
}
