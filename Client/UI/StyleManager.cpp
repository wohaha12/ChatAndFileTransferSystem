#include "StyleManager.h"
#include <QApplication>

StyleManager* StyleManager::s_instance = nullptr;

StyleManager::StyleManager()
    : m_primaryColor(QColor("#2196F3"))
    , m_secondaryColor(QColor("#FF9800"))
    , m_successColor(QColor("#4CAF50"))
    , m_warningColor(QColor("#FFC107"))
    , m_dangerColor(QColor("#F44336"))
    , m_backgroundColor(QColor("#F5F5F5"))
    , m_cardBackgroundColor(QColor("#FFFFFF"))
    , m_textColor(QColor("#333333"))
    , m_secondaryTextColor(QColor("#999999"))
    , m_borderColor(QColor("#E0E0E0"))
    , m_hoverColor(QColor("#F0F0F0"))
    , m_focusColor(QColor("#1976D2"))
{
}

StyleManager::~StyleManager()
{
}

StyleManager* StyleManager::instance()
{
    if (!s_instance) {
        s_instance = new StyleManager();
    }
    return s_instance;
}

QColor StyleManager::primaryColor() const
{
    return m_primaryColor;
}

QColor StyleManager::secondaryColor() const
{
    return m_secondaryColor;
}

QColor StyleManager::successColor() const
{
    return m_successColor;
}

QColor StyleManager::warningColor() const
{
    return m_warningColor;
}

QColor StyleManager::dangerColor() const
{
    return m_dangerColor;
}

QColor StyleManager::backgroundColor() const
{
    return m_backgroundColor;
}

QColor StyleManager::cardBackgroundColor() const
{
    return m_cardBackgroundColor;
}

QColor StyleManager::textColor() const
{
    return m_textColor;
}

QColor StyleManager::secondaryTextColor() const
{
    return m_secondaryTextColor;
}

QColor StyleManager::borderColor() const
{
    return m_borderColor;
}

QColor StyleManager::hoverColor() const
{
    return m_hoverColor;
}

QColor StyleManager::focusColor() const
{
    return m_focusColor;
}

void StyleManager::applyLineEditStyle(QLineEdit* lineEdit)
{
    if (!lineEdit) {
        return;
    }

    QString style = QString(
        "QLineEdit {"
        "   border: 1px solid %1;"
        "   border-radius: 6px;"
        "   padding: 10px 12px;"
        "   background-color: %2;"
        "   color: %3;"
        "   font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        "   border: 2px solid %4;"
        "   outline: none;"
        "}"
        "QLineEdit:hover {"
        "   border: 1px solid %5;"
        "}"
    ).arg(m_borderColor.name(),
           m_cardBackgroundColor.name(),
           m_textColor.name(),
           m_focusColor.name(),
           m_primaryColor.name());

    lineEdit->setStyleSheet(style);
    lineEdit->setMinimumHeight(44);
}

void StyleManager::applyButtonStyle(QPushButton* button, bool isPrimary)
{
    if (!button) {
        return;
    }

    QString bgColor = isPrimary ? m_primaryColor.name() : m_cardBackgroundColor.name();
    QString textColor = isPrimary ? "#FFFFFF" : m_textColor.name();
    QString hoverColor = isPrimary ? "#1976D2" : m_hoverColor.name();
    QString borderColor = isPrimary ? "transparent" : m_borderColor.name();

    QString style = QString(
        "QPushButton {"
        "   background-color: %1;"
        "   color: %2;"
        "   border: 1px solid %3;"
        "   border-radius: 6px;"
        "   padding: 10px 20px;"
        "   font-size: 14px;"
        "   font-weight: 500;"
        "   min-height: 44px;"
        "}"
        "QPushButton:hover {"
        "   background-color: %4;"
        "}"
        "QPushButton:pressed {"
        "   background-color: %5;"
        "}"
        "QPushButton:disabled {"
        "   background-color: %6;"
        "   color: %7;"
        "}"
    ).arg(bgColor,
           textColor,
           borderColor,
           hoverColor,
           isPrimary ? "#1565C0" : "#E0E0E0",
           m_borderColor.name(),
           m_secondaryTextColor.name());

    button->setStyleSheet(style);
}

void StyleManager::applyLabelStyle(QLabel* label, bool isTitle)
{
    if (!label) {
        return;
    }

    if (isTitle) {
        QString style = QString(
            "QLabel {"
            "   color: %1;"
            "   font-size: 24px;"
            "   font-weight: bold;"
            "   padding: 10px;"
            "}"
        ).arg(m_textColor.name());
        label->setStyleSheet(style);
    } else {
        QString style = QString(
            "QLabel {"
            "   color: %1;"
            "   font-size: 14px;"
            "   padding: 5px;"
            "}"
        ).arg(m_textColor.name());
        label->setStyleSheet(style);
    }
}

void StyleManager::applyTreeWidgetStyle(QTreeWidget* treeWidget)
{
    if (!treeWidget) {
        return;
    }

    QString style = QString(
        "QTreeWidget {"
        "   border: 1px solid %1;"
        "   border-radius: 6px;"
        "   background-color: %2;"
        "   color: %3;"
        "   font-size: 14px;"
        "   outline: none;"
        "}"
        "QTreeWidget::item {"
        "   padding: 8px;"
        "   border-bottom: 1px solid %4;"
        "}"
        "QTreeWidget::item:hover {"
        "   background-color: %5;"
        "}"
        "QTreeWidget::item:selected {"
        "   background-color: %6;"
        "   color: white;"
        "}"
        "QTreeWidget::header {"
        "   background-color: %7;"
        "   border-bottom: 2px solid %8;"
        "   padding: 8px;"
        "   font-weight: bold;"
        "}"
    ).arg(m_borderColor.name(),
           m_cardBackgroundColor.name(),
           m_textColor.name(),
           m_borderColor.name(),
           m_hoverColor.name(),
           m_primaryColor.name(),
           m_hoverColor.name(),
           m_borderColor.name());

    treeWidget->setStyleSheet(style);
    treeWidget->setAlternatingRowColors(true);
}

void StyleManager::applyTextEditStyle(QTextEdit* textEdit)
{
    if (!textEdit) {
        return;
    }

    QString style = QString(
        "QTextEdit {"
        "   border: 1px solid %1;"
        "   border-radius: 6px;"
        "   background-color: %2;"
        "   color: %3;"
        "   font-size: 14px;"
        "   padding: 10px;"
        "}"
        "QTextEdit:focus {"
        "   border: 2px solid %4;"
        "   outline: none;"
        "}"
    ).arg(m_borderColor.name(),
           m_cardBackgroundColor.name(),
           m_textColor.name(),
           m_focusColor.name());

    textEdit->setStyleSheet(style);
}

void StyleManager::applyCardStyle(QWidget* widget)
{
    if (!widget) {
        return;
    }

    QString style = QString(
        "QWidget {"
        "   background-color: %1;"
        "   border-radius: 8px;"
        "   border: 1px solid %2;"
        "}"
    ).arg(m_cardBackgroundColor.name(),
           m_borderColor.name());

    widget->setStyleSheet(style);
}

QString StyleManager::getGradientBackground(const QColor& startColor, const QColor& endColor)
{
    return QString("background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                   "stop:0 %1, stop:1 %2);")
           .arg(startColor.name(), endColor.name());
}

QString StyleManager::getShadowStyle(int blurRadius, int offsetX, int offsetY)
{
    return QString("box-shadow: %1px %2px %3px rgba(0, 0, 0, 0.1);")
           .arg(offsetX).arg(offsetY).arg(blurRadius);
}
