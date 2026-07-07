#ifndef NOTIFICATION_WIDGET_H
#define NOTIFICATION_WIDGET_H

#include <QFrame>
#include <QLabel>
#include <QTimer>

class NotificationWidget : public QFrame
{
    Q_OBJECT
public:
    enum Type { Info, Warning, Error };

    explicit NotificationWidget(QWidget *parent = nullptr);

    // 显示一条通知，3秒后自动消失
    void showMessage(const QString &message, Type type = Info);

private slots:
    void hide();

private:
    QLabel *m_label;
    QTimer *m_timer;
};

#endif