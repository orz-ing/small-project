#ifndef BOOK_TOOLTIP_H
#define BOOK_TOOLTIP_H

#include <QFrame>
#include <QLabel>
#include <QTimer>
#include <QTableWidget>
#include "models.h"

// 图书悬浮预览卡片：单例模式，鼠标悬停在书名上时显示详细信息
class BookTooltip : public QFrame {
    Q_OBJECT
public:
    static BookTooltip* instance();

    // 立即显示指定图书的信息
    void showForBook(const Book& book, const QPoint& globalPos);
    // 隐藏卡片
    void hideTooltip();
    // 计划延迟显示（防抖）
    void scheduleShow(int bookId, const QPoint& globalPos);
    void cancelScheduledShow();
    int currentBookId() const { return m_pendingBookId; }

protected:
    void leaveEvent(QEvent* event) override;

private:
    explicit BookTooltip(QWidget* parent = nullptr);
    void setupUI();
    void updateContent(const Book& book);

    QLabel* m_coverLabel;
    QLabel* m_titleLabel;
    QLabel* m_authorLabel;
    QLabel* m_publisherLabel;
    QLabel* m_isbnLabel;
    QLabel* m_categoryLabel;
    QLabel* m_stockLabel;
    QLabel* m_descLabel;

    QTimer* m_showTimer;
    int m_pendingBookId = 0;
    QPoint m_targetPos;

    static constexpr int HOVER_DELAY_MS = 400;
};

// 辅助函数：为 QTableWidget 安装书名悬停预览
// titleColumn: 书名所在的列索引
// idColumn:   图书 ID 所在的列索引；若为 -1，则从书名单元格的 Qt::UserRole 读取 bookId
void installBookTitleHover(QTableWidget* table, int titleColumn, int idColumn);

#endif // BOOK_TOOLTIP_H
