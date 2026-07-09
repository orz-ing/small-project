#include "fine_calculator.h"
#include "utils.h"

void FineCalculator::setTierRate(int days, double rate) {
    // 保持按天数排序
    for (int i = 0; i < m_tiers.size(); ++i) {
        if (m_tiers[i].minDays == days) {
            m_tiers[i].rate = rate;
            return;
        }
    }
    m_tiers.append({days, rate});
    std::sort(m_tiers.begin(), m_tiers.end(),
        [](const Tier& a, const Tier& b) { return a.minDays < b.minDays; });
}

int FineCalculator::calculateOverdueDays(const QDateTime& dueDate, const QDateTime& endDate) const {
    QDateTime end = endDate.isValid() ? endDate : QDateTime::currentDateTime();
    if (end <= dueDate) return 0;

    int overdueDays = 0;
    QDateTime current = dueDate.addDays(1); // 从到期次日开始算
    current.setTime(QTime(0, 0, 0));

    QDateTime endDay(end.date(), QTime(0, 0, 0));

    while (current < endDay) {
        // 跳过节假日/周末
        if (!Utils::isHoliday(current)) {
            overdueDays++;
        }
        current = current.addDays(1);
    }

    return overdueDays;
}

double FineCalculator::calculateFine(const QDateTime& dueDate, const QDateTime& returnDate) const {
    int days = calculateOverdueDays(dueDate, returnDate);
    if (days <= 0) return 0.0;

    // 基础费率
    double rate = m_dailyRate;

    // 检查是否有阶梯费率
    for (int i = m_tiers.size() - 1; i >= 0; --i) {
        if (days >= m_tiers[i].minDays) {
            rate = m_tiers[i].rate;
            break;
        }
    }

    return days * rate;
}
