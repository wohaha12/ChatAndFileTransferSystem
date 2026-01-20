#include "ChatWidget.h"
#include "ui_ChatWidget.h"
#include <QDateTime>

ChatWidget::ChatWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ChatWidget)
{
    ui->setupUi(this);
}

ChatWidget::~ChatWidget()
{
    delete ui;
}

void ChatWidget::addMessage(const QString& sender, const QString& message, bool isSelf)
{
    QString time = QDateTime::currentDateTime().toString("HH:mm:ss");
    QString html;

    if (isSelf) {
        html = QString("<div style='text-align: right; margin: 5px;'>"
                       "<span style='color: gray;'>%1</span> "
                       "<span style='color: blue; font-weight: bold;'>%2</span><br/>"
                       "<span style='background-color: #e6f7ff; padding: 5px; border-radius: 5px;'>%3</span>"
                       "</div>").arg(time, sender, message);
    } else {
        html = QString("<div style='text-align: left; margin: 5px;'>"
                       "<span style='color: blue; font-weight: bold;'>%1</span> "
                       "<span style='color: gray;'>%2</span><br/>"
                       "<span style='background-color: #f0f0f0; padding: 5px; border-radius: 5px;'>%3</span>"
                       "</div>").arg(time, sender, message);
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

    ui->messageLineEdit->clear();
}

void ChatWidget::onMessageTextChanged()
{
    ui->sendButton->setEnabled(!ui->messageLineEdit->text().trimmed().isEmpty());
}
