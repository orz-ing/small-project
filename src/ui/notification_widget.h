#ifndef NOTIFICATION_WIDGET_H
#define NOTIFICATION_WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>

struct Notification {
    QString title;
    QString message;
    QString type; // "info", "warning", "error"
};

class NotificationWidget : public QWidget {
    Q_OBJECT
public:
    static NotificationWidget* instance();

    void showNotification(const QString& title, const QString& message, const QString& type = "info");

private:
    explicit NotificationWidget(QWidget* parent = nullptr);
    void setupUI();
    void fadeOut();

    QVBoxLayout* m_container;
    QTimer* m_fadeTimer;
    static NotificationWidget* s_instance;
};

#endif
