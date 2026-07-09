#include "ui/book_tooltip.h"
#include "bridge/api_bridge.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>
#include <QFontDatabase>

// ============ BookTooltip 单例实现 ============

BookTooltip* BookTooltip::instance() {
    static BookTooltip inst;
    return &inst;
}

BookTooltip::BookTooltip(QWidget* parent)
    : QFrame(parent, Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint) {
    setupUI();
    setAttribute(Qt::WA_ShowWithoutActivating);
    setMouseTracking(true);
}

void BookTooltip::setupUI() {
    setObjectName("card");
    setFixedWidth(400);
    setStyleSheet(
        "QFrame#card {"
        "  background-color: #ffffff;"
        "  border-radius: 12px;"
        "  border: 1px solid #dce1e8;"
        "  padding: 16px;"
        "}"
    );

    // 添加阴影效果
    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setOffset(2, 4);
    shadow->setColor(QColor(0, 0, 0, 60));
    setGraphicsEffect(shadow);

    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(16, 14, 16, 14);
    mainLayout->setSpacing(14);

    // 左侧：封面卡片
    m_coverLabel = new QLabel;
    m_coverLabel->setFixedSize(110, 150);
    m_coverLabel->setAlignment(Qt::AlignCenter);
    m_coverLabel->setWordWrap(true);
    mainLayout->addWidget(m_coverLabel);

    // 右侧：信息区域
    auto* infoLayout = new QVBoxLayout;
    infoLayout->setSpacing(4);

    m_titleLabel = new QLabel;
    m_titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #14142b;");
    m_titleLabel->setWordWrap(true);
    m_titleLabel->setMaximumWidth(240);
    infoLayout->addWidget(m_titleLabel);

    m_authorLabel = new QLabel;
    m_authorLabel->setWordWrap(true);
    m_authorLabel->setMaximumWidth(240);
    infoLayout->addWidget(m_authorLabel);

    m_publisherLabel = new QLabel;
    m_publisherLabel->setWordWrap(true);
    m_publisherLabel->setMaximumWidth(240);
    infoLayout->addWidget(m_publisherLabel);

    m_isbnLabel = new QLabel;
    m_isbnLabel->setWordWrap(true);
    m_isbnLabel->setMaximumWidth(240);
    infoLayout->addWidget(m_isbnLabel);

    m_categoryLabel = new QLabel;
    m_categoryLabel->setWordWrap(true);
    m_categoryLabel->setMaximumWidth(240);
    infoLayout->addWidget(m_categoryLabel);

    m_stockLabel = new QLabel;
    m_stockLabel->setWordWrap(true);
    m_stockLabel->setMaximumWidth(240);
    infoLayout->addWidget(m_stockLabel);

    infoLayout->addSpacing(4);

    m_descLabel = new QLabel;
    m_descLabel->setStyleSheet("font-size: 12px; color: #4a5a72;");
    m_descLabel->setWordWrap(true);
    m_descLabel->setMaximumWidth(240);
    m_descLabel->setMaximumHeight(55);
    infoLayout->addWidget(m_descLabel);

    infoLayout->addStretch();
    mainLayout->addLayout(infoLayout);

    // 定时器：延迟显示
    m_showTimer = new QTimer(this);
    m_showTimer->setSingleShot(true);
    connect(m_showTimer, &QTimer::timeout, this, [this]() {
        if (m_pendingBookId > 0) {
            Book book = ApiBridge::instance()->getBookDetail(m_pendingBookId);
            if (book.id > 0) {
                updateContent(book);
                showForBook(book, m_targetPos);
            }
        }
    });
}

void BookTooltip::updateContent(const Book& book) {
    m_titleLabel->setText(book.title);

    m_authorLabel->setText(
        QString("<span style='color:#6b7c93;font-size:12px;'>作者: </span>"
                "<span style='color:#1a2332;font-size:12px;'>%1</span>")
            .arg(book.author.isEmpty() ? "未知" : book.author.toHtmlEscaped()));

    m_publisherLabel->setText(
        QString("<span style='color:#6b7c93;font-size:12px;'>出版社: </span>"
                "<span style='color:#1a2332;font-size:12px;'>%1</span>")
            .arg(book.publisher.isEmpty() ? "未知" : book.publisher.toHtmlEscaped()));

    m_isbnLabel->setText(
        QString("<span style='color:#6b7c93;font-size:12px;'>ISBN: </span>"
                "<span style='color:#1a2332;font-size:12px;'>%1</span>")
            .arg(book.isbn.isEmpty() ? "无" : book.isbn));

    m_categoryLabel->setText(
        QString("<span style='color:#6b7c93;font-size:12px;'>分类: </span>"
                "<span style='color:#1a2332;font-size:12px;'>%1</span>")
            .arg(book.categoryPath.isEmpty() ? "未分类" : book.categoryPath));

    // 库存状态颜色
    QString stockColor = (book.availableStock <= 0) ? "#e74c3c" :
                         (book.availableStock <= 2) ? "#f39c12" : "#27ae60";
    m_stockLabel->setText(
        QString("<span style='color:#6b7c93;font-size:12px;'>库存: </span>"
                "<span style='color:%1;font-size:12px;font-weight:bold;'>%2 可借</span>"
                "<span style='color:#6b7c93;font-size:12px;'> / 共 %3 本</span>")
            .arg(stockColor)
            .arg(book.availableStock)
            .arg(book.totalStock));

    // 描述截断
    QString desc = book.description.trimmed();
    if (desc.length() > 120) {
        desc = desc.left(120) + "...";
    }
    m_descLabel->setText(
        QString("<span style='color:#4a5a72;font-size:12px;'>%1</span>")
            .arg(desc.isEmpty() ? "暂无简介" : desc.toHtmlEscaped()));

    // 封面：真实图片 或 精美占位卡片
    bool hasCoverImage = false;
    if (!book.coverPath.isEmpty()) {
        QPixmap cover(book.coverPath);
        if (!cover.isNull()) {
            m_coverLabel->setPixmap(cover.scaled(110, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            m_coverLabel->setStyleSheet(
                "QLabel { border-radius: 8px; border: 1px solid #dce1e8; }");
            m_coverLabel->setText("");
            hasCoverImage = true;
        }
    }

    if (!hasCoverImage) {
        // 用 QPainter 绘制精美占位封面
        uint hash = qHash(book.title);
        const QColor colorSets[][2] = {
            {QColor("#1a3d6b"), QColor("#2d6a9f")},  // 深蓝
            {QColor("#7b1d3d"), QColor("#b82d5a")},  // 玫红
            {QColor("#1a6b3d"), QColor("#2d9f5e")},  // 翠绿
            {QColor("#6b501a"), QColor("#9f7a2d")},  // 金棕
            {QColor("#3d1a6b"), QColor("#6a2d9f")},  // 紫罗兰
            {QColor("#1a506b"), QColor("#2d809f")},  // 青蓝
            {QColor("#6b301a"), QColor("#9f502d")},  // 暖橙
            {QColor("#4a1a6b"), QColor("#7a2d9f")},  // 深紫
            {QColor("#1a6b5a"), QColor("#2d9f82")},  // 松石绿
            {QColor("#556b1a"), QColor("#8a9f2d")},  // 橄榄绿
        };
        int ci = hash % 10;
        QColor topColor = colorSets[ci][0];
        QColor botColor = colorSets[ci][1];

        QPixmap coverPix(110, 150);
        coverPix.fill(Qt::transparent);
        QPainter p(&coverPix);
        p.setRenderHint(QPainter::Antialiasing);

        // 圆角矩形背景
        QPainterPath bgPath;
        bgPath.addRoundedRect(0, 0, 110, 150, 8, 8);
        QLinearGradient grad(0, 0, 0, 150);
        grad.setColorAt(0, topColor);
        grad.setColorAt(1, botColor);
        p.fillPath(bgPath, grad);

        // 书脊装饰
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255, 255, 255, 30));
        p.drawRoundedRect(6, 10, 4, 130, 2, 2);

        // 装饰横线
        p.setBrush(QColor(255, 255, 255, 50));
        p.drawRoundedRect(20, 80, 70, 1, 1, 1);

        // 书名大字
        QString coverTitle = book.title.trimmed();
        if (coverTitle.length() > 6) coverTitle = coverTitle.left(6);
        QFont titleFont("Microsoft YaHei", 16, QFont::Bold);
        p.setFont(titleFont);
        p.setPen(QColor(255, 255, 255));
        // 拆分两行
        if (coverTitle.length() > 3) {
            int mid = coverTitle.length() / 2;
            // 按整字拆分（中文每字一个字符）
            QString line1 = coverTitle.left(mid);
            QString line2 = coverTitle.mid(mid);
            p.drawText(QRectF(8, 18, 94, 30), Qt::AlignHCenter | Qt::AlignBottom, line1);
            p.drawText(QRectF(8, 44, 94, 28), Qt::AlignHCenter | Qt::AlignTop, line2);
        } else {
            p.drawText(QRectF(8, 24, 94, 50), Qt::AlignCenter, coverTitle);
        }

        // 作者小字
        QString authorShort = book.author;
        if (authorShort.length() > 6) authorShort = authorShort.left(6) + "…";
        QFont authorFont("Microsoft YaHei", 8);
        p.setFont(authorFont);
        p.setPen(QColor(255, 255, 255, 200));
        p.drawText(QRectF(8, 110, 94, 30), Qt::AlignCenter, authorShort);

        p.end();

        m_coverLabel->setPixmap(coverPix);
        m_coverLabel->setStyleSheet(
            "QLabel { border-radius: 8px; border: none; }");
        m_coverLabel->setText("");
    }

    adjustSize();
}

void BookTooltip::showForBook(const Book& book, const QPoint& globalPos) {
    updateContent(book);

    // 计算位置：光标右侧偏移，保持在屏幕范围内
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeo = screen ? screen->availableGeometry() : QRect(0, 0, 1920, 1080);

    int x = globalPos.x() + 18;
    int y = globalPos.y() - 20;

    // 右边界检查：如果超出屏幕右侧，则显示在光标左侧
    if (x + width() > screenGeo.right()) {
        x = globalPos.x() - width() - 18;
    }
    // 左边界检查
    if (x < screenGeo.left()) {
        x = screenGeo.left() + 4;
    }
    // 下边界检查
    if (y + height() > screenGeo.bottom()) {
        y = screenGeo.bottom() - height() - 4;
    }
    // 上边界检查
    if (y < screenGeo.top()) {
        y = screenGeo.top() + 4;
    }

    move(x, y);
    show();
    raise();
}

void BookTooltip::hideTooltip() {
    hide();
    m_pendingBookId = 0;
}

void BookTooltip::scheduleShow(int bookId, const QPoint& globalPos) {
    if (m_pendingBookId == bookId && isVisible()) {
        return;  // 同一本书，已经在显示
    }
    m_pendingBookId = bookId;
    m_targetPos = globalPos;
    m_showTimer->start(HOVER_DELAY_MS);
}

void BookTooltip::cancelScheduledShow() {
    m_showTimer->stop();
    if (isVisible()) {
        hideTooltip();
    }
}

void BookTooltip::leaveEvent(QEvent* event) {
    Q_UNUSED(event);
    hideTooltip();
}

// ============ HoverFilter 内部类 ============

namespace {

class BookTitleHoverFilter : public QObject {
public:
    BookTitleHoverFilter(QTableWidget* table, int titleColumn, int idColumn)
        : QObject(table), m_table(table), m_titleCol(titleColumn), m_idCol(idColumn) {}

protected:
    bool eventFilter(QObject* watched, QEvent* event) override {
        if (event->type() == QEvent::MouseMove) {
            auto* me = static_cast<QMouseEvent*>(event);
            QPoint viewportPos = m_table->viewport()->mapFromGlobal(me->globalPosition().toPoint());
            int row = m_table->rowAt(viewportPos.y());
            int col = m_table->columnAt(viewportPos.x());

            if (row >= 0 && row < m_table->rowCount() && col == m_titleCol) {
                int bookId = 0;

                if (m_idCol >= 0) {
                    // 从隐藏 ID 列读取
                    auto* idItem = m_table->item(row, m_idCol);
                    if (idItem) {
                        bookId = idItem->text().toInt();
                    }
                } else {
                    // 从书名单元格的 UserRole 读取
                    auto* titleItem = m_table->item(row, m_titleCol);
                    if (titleItem) {
                        bookId = titleItem->data(Qt::UserRole).toInt();
                    }
                }

                if (bookId > 0) {
                    BookTooltip::instance()->scheduleShow(bookId, me->globalPosition().toPoint());
                } else {
                    BookTooltip::instance()->cancelScheduledShow();
                }
            } else {
                BookTooltip::instance()->cancelScheduledShow();
            }
        } else if (event->type() == QEvent::Leave) {
            BookTooltip::instance()->cancelScheduledShow();
        }
        return QObject::eventFilter(watched, event);
    }

private:
    QTableWidget* m_table;
    int m_titleCol;
    int m_idCol;
};

} // anonymous namespace

// ============ 公开辅助函数 ============

void installBookTitleHover(QTableWidget* table, int titleColumn, int idColumn) {
    if (!table) return;

    table->viewport()->setMouseTracking(true);
    table->viewport()->installEventFilter(
        new BookTitleHoverFilter(table, titleColumn, idColumn));
}
