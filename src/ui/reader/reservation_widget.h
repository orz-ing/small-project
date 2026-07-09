#ifndef RESERVATION_WIDGET_H
#define RESERVATION_WIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>

class ReservationWidget : public QWidget {
    Q_OBJECT
public:
    explicit ReservationWidget(QWidget* parent = nullptr);
private slots:
    void refreshData();
    void onCancelReservation(int reservationId);
private:
    void setupUI();
    QTableWidget* m_table;
    QLabel* m_infoLabel;
};
#endif
