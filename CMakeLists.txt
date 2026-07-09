cmake_minimum_required(VERSION 3.16)
project(LibraryManagementSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 指定Qt路径
set(CMAKE_PREFIX_PATH "D:/Qt/6.9.3/msvc2022_64")

# 查找Qt模块
find_package(Qt6 REQUIRED COMPONENTS Widgets Charts Sql)

# 源文件
set(SOURCES
    src/main.cpp
    src/utils.cpp
    src/engine/search_index.cpp
    src/engine/recommend_engine.cpp
    src/engine/fine_calculator.cpp
    src/engine/category_tree.cpp
    src/engine/reservation_queue.cpp
    src/engine/credit_engine.cpp
    src/dao/database_manager.cpp
    src/dao/book_dao.cpp
    src/dao/user_dao.cpp
    src/dao/borrow_record_dao.cpp
    src/dao/reservation_dao.cpp
    src/dao/category_dao.cpp
    src/dao/log_dao.cpp
    src/service/auth_service.cpp
    src/service/book_service.cpp
    src/service/borrow_service.cpp
    src/service/reservation_service.cpp
    src/service/overdue_scanner.cpp
    src/service/log_service.cpp
    src/bridge/api_bridge.cpp
    src/ui/login_dialog.cpp
    src/ui/main_window.cpp
    src/ui/reader/reader_main_widget.cpp
    src/ui/reader/search_widget.cpp
    src/ui/reader/borrow_panel.cpp
    src/ui/reader/recommend_widget.cpp
    src/ui/reader/reservation_widget.cpp
    src/ui/admin/admin_dashboard.cpp
    src/ui/admin/book_manager_widget.cpp
    src/ui/admin/user_manager_widget.cpp
    src/ui/admin/statistics_widget.cpp
    src/ui/notification_widget.cpp
    tools/mock_generator.cpp
    tools/csv_exporter.cpp
    tools/json_exporter.cpp
)

set(HEADERS
    src/models.h
    src/utils.h
    src/engine/search_index.h
    src/engine/recommend_engine.h
    src/engine/fine_calculator.h
    src/engine/category_tree.h
    src/engine/reservation_queue.h
    src/engine/credit_engine.h
    src/dao/database_manager.h
    src/dao/book_dao.h
    src/dao/user_dao.h
    src/dao/borrow_record_dao.h
    src/dao/reservation_dao.h
    src/dao/category_dao.h
    src/dao/log_dao.h
    src/service/auth_service.h
    src/service/book_service.h
    src/service/borrow_service.h
    src/service/reservation_service.h
    src/service/overdue_scanner.h
    src/service/log_service.h
    src/bridge/api_bridge.h
    src/ui/login_dialog.h
    src/ui/main_window.h
    src/ui/reader/reader_main_widget.h
    src/ui/reader/search_widget.h
    src/ui/reader/borrow_panel.h
    src/ui/reader/recommend_widget.h
    src/ui/reader/reservation_widget.h
    src/ui/admin/admin_dashboard.h
    src/ui/admin/book_manager_widget.h
    src/ui/admin/user_manager_widget.h
    src/ui/admin/statistics_widget.h
    src/ui/notification_widget.h
    tools/mock_generator.h
    tools/csv_exporter.h
    tools/json_exporter.h
)

# 资源文件
set(RESOURCES
    resources/resources.qrc
)

# 可执行文件
add_executable(LibraryManagementSystem
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# 链接Qt库
target_link_libraries(LibraryManagementSystem PRIVATE
    Qt6::Widgets
    Qt6::Charts
    Qt6::Sql
)

# 包含目录
target_include_directories(LibraryManagementSystem PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 启用编译器警告
if(MSVC)
    target_compile_options(LibraryManagementSystem PRIVATE /W4 /utf-8)
else()
    target_compile_options(LibraryManagementSystem PRIVATE -Wall -Wextra)
endif()
