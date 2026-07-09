#include "reservation_widget.h"
#include "bridge/api_bridge.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMessageBox>

ReservationWidget::ReservationWidget(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void ReservationWidget::setupUI() {
    auto* layout = new QVBoxLayout(this);

    auto* title = new QLabel("📋 我的预约");
    title->setObjectName("sectionTitle");
    layout->addWidget(title);

    auto* refreshBtn = new QPushButton("🔄 刷新");
    refreshBtn->setObjectName("primaryBtn");
    connect(refreshBtn, &QPushButton::clicked, this, &ReservationWidget::refreshData);
    layout->addWidget(refreshBtn);

    m_infoLabel = new QLabel;
    m_infoLabel->setStyleSheet("color: #7f8c8d; padding: 5px 0;");
    layout->addWidget(m_infoLabel);

    m_table = new QTableWidget;
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels({"预约ID", "书名", "预约时间", "过期时间", "状态", "操作"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setColumnHidden(0, true);
    m_table->verticalHeader()->hide();
    m_table->horizontalHeader()->setStretchLastSection(true);
    layout->addWidget(m_table, 1);

    refreshData();
}

void ReservationWidget::refreshData() {
    int userId = ApiBridge::instance()->currentUser().id;
    auto reservations = ApiBridge::instance()->getMyReservations(userId);

    m_table->setRowCount(reservations.size());
    int pendingCount = 0;

    for (int i = 0; i < reservations.size(); ++i) {
        auto& r = reservations[i];
        m_table->setItem(i, 0, new QTableWidgetItem(QString::number(r.id)));
        m_table->setItem(i, 1, new QTableWidgetItem(r.bookTitle));
        m_table->setItem(i, 2, new QTableWidgetItem(r.reserveDate.toString("yyyy-MM-dd HH:mm")));
        m_table->setItem(i, 3, new QTableWidgetItem(r.expireDate.toString("yyyy-MM-dd HH:mm")));

        QString statusText;
        switch (r.status) {
            case ReservationStatus::Pending: statusText = "⏳ 等待中"; pendingCount++; break;
            case ReservationStatus::Notified: statusText = "✅ 可借阅"; break;
            case ReservationStatus::Cancelled: statusText = "❌ 已取消"; break;
            case ReservationStatus::Expired: statusText = "⏰ 已过期"; break;
            case ReservationStatus::Fulfilled: statusText = "📖 已完成"; break;
        }
        m_table->setItem(i, 4, new QTableWidgetItem(statusText));

        if (r.status == ReservationStatus::Pending || r.status == ReservationStatus::Notified) {
            auto* cancelBtn = new QPushButton("取消预约");
            cancelBtn->setObjectName("dangerBtn");
            int resId = r.id;
            connect(cancelBtn, &QPushButton::clicked, this, [this, resId]() { onCancelReservation(resId); });
            m_table->setCellWidget(i, 5, cancelBtn);
        }
    }
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_infoLabel->setText(QString("共 %1 条预约记录，%2 条等待中").arg(reservations.size()).arg(pendingCount));
}

void ReservationWidget::onCancelReservation(int reservationId) {
    auto reply = QMessageBox::question(this, "确认取消", "确定要取消该预约吗？");
    if (reply != QMessageBox::Yes) return;

    Result r = ApiBridge::instance()->cancelReservation(reservationId);
    if (r.success) {
        QMessageBox::information(this, "成功", "预约已取消");
        refreshData();
    } else {
        QMessageBox::warning(this, "失败", r.message);
    }
}
