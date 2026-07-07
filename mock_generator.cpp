// mock_generator.cpp — Mock 数据生成器（独立可执行）
// 用法: ./mock_generator <图书数> <用户数> <借阅数> <预约数> <分类数>
#include "backend.h"
using namespace std;

int main(int argc, char* argv[])
{
    // TODO:
    // 1. 解析命令行参数 → nBooks, nUsers, nBorrows, nReservations, nCategories
    // 2. 初始化 DatabaseManager → 清空旧数据
    // 3. 生成分类: 40个随机分类（含层级关系）
    // 4. 生成图书: 随机 isbn/书名/作者/出版社 → BookDao::insert()
    // 5. 生成用户: 随机用户名/密码("123456") → UserDao::insert() → 至少1个admin
    // 6. 生成借阅: 随机 user-book 配对 → BorrowRecordDao::insert()
    // 7. 生成预约: 随机 user-book(库存为0的) 配对 → ReservationDao::insert()
    // 8. 输出统计信息: 生成总数
}
