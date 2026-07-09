#ifndef FINE_CALCULATOR_H
#define FINE_CALCULATOR_H

#include <QDateTime>

// 罚款计算器：逾期天数 × 日费率 + 节假日跳过 + 阶梯费率
class FineCalculator {
public:
    FineCalculator() = default;

    // 设置日费率（默认0.5元/天）
    void setDailyRate(double rate) { m_dailyRate = rate; }
    double dailyRate() const { return m_dailyRate; }

    // 设置阶梯费率（天数→费率映射，天数越大费率越高）
    void setTierRate(int days, double rate);

    // 计算罚款
    // dueDate: 应还日期, returnDate: 实际归还日期（为空表示当前时间）
    double calculateFine(const QDateTime& dueDate, const QDateTime& returnDate = QDateTime()) const;

    // 计算逾期天数（跳过节假日/周末）
    int calculateOverdueDays(const QDateTime& dueDate, const QDateTime& endDate = QDateTime()) const;

private:
    double m_dailyRate = 0.5;
    struct Tier {
        int minDays;
        double rate;
    };
    QVector<Tier> m_tiers;
};

#endif
