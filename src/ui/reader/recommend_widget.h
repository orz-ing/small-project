#ifndef RECOMMEND_WIDGET_H
#define RECOMMEND_WIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>

class RecommendWidget : public QWidget {
    Q_OBJECT
public:
    explicit RecommendWidget(QWidget* parent = nullptr);
private slots:
    void refreshRecommendations();
private:
    void setupUI();
    QTableWidget* m_table;
    QLabel* m_infoLabel;
};
#endif
