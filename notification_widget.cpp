#include "notification_widget.h"
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

NotificationWidget::NotificationWidget(QWidget *parent)
    : QFrame(parent)
{
    setObjectName("notificationBar");
    setFixedHeight(45);
    setVisible(false);

    // 布局
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(15, 0, 15, 0);

    m_label = new QLabel(this);
    m_label->setObjectName("notificationLabel");
    layout->addWidget(m_label, 1);

    // 自动隐藏定时器
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &NotificationWidget::hide);

    // 默认样式
    setStyleSheet(
        "QFrame#notificationBar { background-color: #323232; border-radius: 4px; }"
        "QLabel#notificationLabel { color: white; font-size: 13px; padding: 8px; }"
        );
}

void NotificationWidget::showMessage(const QString &message, Type type)
{
    // 根据类型设置背景色
    QString bgColor;
    switch (type) {
    case Warning: bgColor = "#f57c00"; break;
    case Error:   bgColor = "#d32f2f"; break;
    default:      bgColor = "#323232"; break;
    }
    setStyleSheet(
        QString("QFrame#notificationBar { background-color: %1; border-radius: 4px; }"
                "QLabel#notificationLabel { color: white; font-size: 13px; padding: 8px; }")
            .arg(bgColor)
        );

    m_label->setText(message);
    show();
    raise();  // 确保在最前

    // 重新启动定时器
    m_timer->start(3000);
}

void NotificationWidget::hide()
{
    QFrame::hide();
}