/******************************************************************************
 * frontend.h — 图书管理系统前端统一头文件
 *
 * 依赖：Qt 6 Widgets, Qt Charts, QSS
 ******************************************************************************/

#ifndef FRONTEND_H
#define FRONTEND_H

// ========== Qt 6 核心头文件 ==========
#include <QApplication>
#include <QWidget>
#include <QDialog>
#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QTableWidget>
#include <QComboBox>
#include <QDateEdit>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QStackedWidget>
#include <QListWidget>
#include <QScrollArea>
#include <QGroupBox>
#include <QTimer>
#include <QSettings>
#include <QPropertyAnimation>
#include <QJsonArray>
#include <QJsonObject>
// Qt Charts
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QLineSeries>

using namespace std;


// ============================================================
//  第 1 部分 — 登录/注册对话框  (~450行)
//  文件: login_dialog.h/.cpp/.ui
// ============================================================
// 【待实现内容】
//   - UI: 账号输入框(QLineEdit)、密码输入框(QLineEdit, echo=Password)
//         登录按钮(QPushButton)、注册按钮(QPushButton)
//         角色选择(QComboBox: 读者/管理员)
//   - 槽函数:
//       onLoginClicked()  → 调用 ApiBridge::login() → 根据角色跳转
//       onRegisterClicked() → 弹出注册表单 → ApiBridge::register()
//   - 输入校验: 账号密码非空、密码长度≥6
//   - 错误提示: QMessageBox 显示服务器返回错误
//   - 记住密码: QCheckBox + QSettings 持久化


// ============================================================
//  第 2 部分 — 主窗口框架  (~300行)
//  文件: main_window.h/.cpp/.ui
// ============================================================
// 【待实现内容】
//   - 菜单栏: 文件(退出)、帮助(关于)
//   - 状态栏: 显示当前用户、日期
//   - 中央区域: QStackedWidget 切换不同功能页
//   - 登录成功后切换到读者主界面或管理员仪表盘
//   - 退出登录 → 返回登录对话框


// ============================================================
//  第 3 部分 — 读者主界面  (~200行)
//  文件: reader_main_widget.h/.cpp
// ============================================================
// 【待实现内容】
//   - 左侧导航栏(QListWidget): 图书检索、我的借阅、智能推荐、我的预约
//   - 右侧内容区(QStackedWidget): 对应四个功能子页面
//   - 欢迎信息: 显示读者姓名、信用积分
//   - 子模块:
//       图书检索    → search_widget
//       我的借阅    → borrow_panel
//       智能推荐    → recommend_widget
//       我的预约    → reservation_widget


// ============================================================
//  第 4 部分 — 图书检索  (~400行)
//  文件: search_widget.h/.cpp
// ============================================================
// 【待实现内容】
//   - 搜索栏: QLineEdit 输入关键词 + QPushButton 搜索
//   - 筛选条件: QComboBox(分类)、QComboBox(排序: 相关度/出版年份/评分)
//   - 结果展示: QTableWidget(封面、书名、作者、出版社、库存、操作)
//   - 操作列: 借阅按钮(库存>0时可用)、预约按钮(库存=0时可用)
//   - 分页: 上一页/下一页按钮
//   - 调用 ApiBridge::searchBooks() → 后端倒排索引 SearchIndex


// ============================================================
//  第 5 部分 — 我的借阅  (~350行)
//  文件: borrow_panel.h/.cpp
// ============================================================
// 【待实现内容】
//   - 借阅列表: QTableWidget(书名、借阅日期、应还日期、状态、操作)
//   - 状态标签: 正常(绿色)、即将到期(黄色)、已逾期(红色)
//   - 操作按钮: 续借(每本书限1次)、归还
//   - 调用 ApiBridge::getMyBorrowRecords() / renewBook() / returnBook()
//   - 逾期提醒: 红色高亮 + 罚款金额显示


// ============================================================
//  第 6 部分 — 智能推荐  (~250行)
//  文件: recommend_widget.h/.cpp
// ============================================================
// 【待实现内容】
//   - 推荐卡片列表: 用 QScrollArea + 水平布局展示推荐书籍
//   - 每张卡片: 封面图片(QLabel pixmap)、书名、作者、借阅按钮
//   - 调用 ApiBridge::getRecommendations() → 后端协同过滤 RecommendEngine
//   - 刷新按钮: 重新获取推荐
//   - 空状态: 无推荐时显示提示文字


// ============================================================
//  第 7 部分 — 我的预约  (~250行)
//  文件: reservation_widget.h/.cpp
// ============================================================
// 【待实现内容】
//   - 预约列表: QTableWidget(书名、预约时间、排队位置、状态、操作)
//   - 状态: 排队中、可取阅(48h内)、已过期
//   - 操作: 取消预约按钮
//   - 调用 ApiBridge::getMyReservations() / cancelReservation()
//   - 定时刷新: QTimer 每30秒自动刷新


// ============================================================
//  第 8 部分 — 管理员仪表盘  (~300行)
//  文件: admin_dashboard.h/.cpp
// ============================================================
// 【待实现内容】
//   - 概览统计: 总图书数、总用户数、活跃借阅数、逾期数、总罚款(QLabel 显示)
//   - 功能入口按钮: 图书管理、用户管理、数据统计
//   - 调用 ApiBridge::getStatistics() 获取统计数据
//   - 点击按钮切换右侧 QStackedWidget 到对应管理页面


// ============================================================
//  第 9 部分 — 图书管理  (~400行)
//  文件: book_manager_widget.h/.cpp
// ============================================================
// 【待实现内容】
//   - 图书列表: QTableWidget(序号、书名、作者、ISBN、分类、库存、操作)
//   - 搜索/筛选: 按书名/ISBN/分类筛选
//   - 新增图书: 弹出对话框填写书籍信息 → ApiBridge::addBook()
//   - 编辑图书: 双击行 → 弹出编辑对话框 → ApiBridge::updateBook()
//   - 删除图书: 选中行 → 确认对话框 → ApiBridge::deleteBook()
//   - 库存管理: 增减库存按钮
//   - 导入/导出: CSV 导入批量添加图书 → CsvExporter 导出


// ============================================================
//  第 10 部分 — 用户管理  (~350行)
//  文件: user_manager_widget.h/.cpp
// ============================================================
// 【待实现内容】
//   - 用户列表: QTableWidget(姓名、账号、角色、信用分、借阅数、状态、操作)
//   - 搜索: 按姓名/账号搜索
//   - 禁用/启用: 切换用户状态按钮 → ApiBridge::disableUser()/enableUser()
//   - 查看详情: 双击行查看用户借阅历史、信用变动记录
//   - 重置密码: 管理员重置用户密码


// ============================================================
//  第 11 部分 — 数据统计  (~400行)
//  文件: statistics_widget.h/.cpp
//  依赖: Qt Charts
// ============================================================
// 【待实现内容】
//   - 柱状图: 各分类图书数量分布 (QBarSeries)
//   - 饼图:   借阅状态占比（在借/已还/逾期）(QPieSeries)
//   - 折线图: 近30天每日借阅量趋势 (QLineSeries)
//   - 图表切换: QComboBox 选择显示的图表类型
//   - 数据来源: ApiBridge::getStatistics() + 按分类统计
//   - 导出: 导出统计报表为 PDF/CSV


// ============================================================
//  第 12 部分 — 弹窗通知中心  (~200行)
//  文件: notification_widget.h/.cpp
// ============================================================
// 【待实现内容】
//   - 右下角弹出式通知窗口（类似 Toast）
//   - 通知类型: 借阅成功、预约到书、逾期提醒、系统公告
//   - 动画: 淡入淡出效果 (QPropertyAnimation)
//   - 通知队列: 多条通知依次显示
//   - 自动消失: QTimer 3秒后自动关闭
//   - 静态方法: NotificationWidget::show(message, type) 全局调用


// ============================================================
//  第 13 部分 — 全局主题样式
//  文件: style.qss
// ============================================================
// 【待填写内容】
//   - 全局字体: "Microsoft YaHei", 14px
//   - 按钮: 圆角 6px, 蓝底白字, hover变色
//   - 输入框: 圆角 4px, 浅灰边框, focus高亮
//   - 表格: 交替行颜色, 表头深色背景
//   - 侧边栏: 宽度240px, 深蓝背景
//   - 对话框: 最小宽度400px
//   - 加载: main.cpp 中通过 qApp->setStyleSheet() 加载

#endif // FRONTEND_H
