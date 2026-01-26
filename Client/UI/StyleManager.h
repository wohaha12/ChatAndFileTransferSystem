#ifndef CLIENT_UI_STYLEMANAGER_H
#define CLIENT_UI_STYLEMANAGER_H

#include <QColor>
#include <QString>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTreeWidget>
#include <QTextEdit>

class StyleManager
{
public:
    static StyleManager* instance();

    QColor primaryColor() const;
    QColor secondaryColor() const;
    QColor successColor() const;
    QColor warningColor() const;
    QColor dangerColor() const;
    QColor backgroundColor() const;
    QColor cardBackgroundColor() const;
    QColor textColor() const;
    QColor secondaryTextColor() const;
    QColor borderColor() const;
    QColor hoverColor() const;
    QColor focusColor() const;

    void applyLineEditStyle(QLineEdit* lineEdit);
    void applyButtonStyle(QPushButton* button, bool isPrimary = false);
    void applyLabelStyle(QLabel* label, bool isTitle = false);
    void applyTreeWidgetStyle(QTreeWidget* treeWidget);
    void applyTextEditStyle(QTextEdit* textEdit);
    void applyCardStyle(QWidget* widget);

    QString getGradientBackground(const QColor& startColor, const QColor& endColor);
    QString getShadowStyle(int blurRadius = 10, int offsetX = 0, int offsetY = 2);

private:
    StyleManager();
    ~StyleManager();

    QColor m_primaryColor;
    QColor m_secondaryColor;
    QColor m_successColor;
    QColor m_warningColor;
    QColor m_dangerColor;
    QColor m_backgroundColor;
    QColor m_cardBackgroundColor;
    QColor m_textColor;
    QColor m_secondaryTextColor;
    QColor m_borderColor;
    QColor m_hoverColor;
    QColor m_focusColor;

    static StyleManager* s_instance;
};

#endif // CLIENT_UI_STYLEMANAGER_H
