#include "notification_widget.h"
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QApplication>

NotificationWidget* NotificationWidget::s_instance = nullptr;

NotificationWidget* NotificationWidget::instance() {
    if (!s_instance) {
        s_instance = new NotificationWidget;
    }
    return s_instance;
}

NotificationWidget::NotificationWidget(QWidget* parent)
    : QWidget(parent) {
    setupUI();
}

void NotificationWidget::setupUI() {
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedWidth(350);
    setObjectName("notification");

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    m_container = new QVBoxLayout;
    layout->addLayout(m_container);
    layout->addStretch();

    m_fadeTimer = new QTimer(this);
    m_fadeTimer->setSingleShot(true);
    connect(m_fadeTimer, &QTimer::timeout, this, &NotificationWidget::fadeOut);
}

void NotificationWidget::showNotification(const QString& title, const QString& message, const QString& type) {
    auto* notifFrame = new QFrame;
    notifFrame->setObjectName("card");
    notifFrame->setStyleSheet(
        "QFrame#card { background-color: " +
        (type == "error" ? QString("#e74c3c") :
         type == "warning" ? QString("#f39c12") : QString("#2ecc71")) +
        "; color: white; border-radius: 8px; padding: 12px; margin: 4px; }"
    );

    auto* frameLayout = new QVBoxLayout(notifFrame);
    auto* titleLabel = new QLabel("<b>" + title + "</b>");
    titleLabel->setStyleSheet("color: white; font-size: 14px;");
    frameLayout->addWidget(titleLabel);

    auto* msgLabel = new QLabel(message);
    msgLabel->setStyleSheet("color: white; font-size: 12px;");
    msgLabel->setWordWrap(true);
    frameLayout->addWidget(msgLabel);

    m_container->addWidget(notifFrame);

    // Auto remove after 4 seconds
    QTimer::singleShot(4000, this, [this, notifFrame]() {
        auto* effect = new QGraphicsOpacityEffect(notifFrame);
        notifFrame->setGraphicsEffect(effect);
        auto* anim = new QPropertyAnimation(effect, "opacity");
        anim->setDuration(500);
        anim->setStartValue(1.0);
        anim->setEndValue(0.0);
        connect(anim, &QPropertyAnimation::finished, [notifFrame]() { notifFrame->deleteLater(); });
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    });

    // Position at top-right of parent
    if (parentWidget()) {
        QPoint topRight = parentWidget()->mapToGlobal(QPoint(parentWidget()->width(), 0));
        move(topRight.x() - width() - 10, topRight.y() + 10);
    }
    show();
}

void NotificationWidget::fadeOut() {
    hide();
}
