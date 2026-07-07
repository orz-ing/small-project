cmake_minimum_required(VERSION 3.16)
project(LibraryManagementSystem VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(SQLite3 REQUIRED)
include_directories(${CMAKE_SOURCE_DIR} ${SQLite3_INCLUDE_DIRS})

add_library(library_core STATIC
    database_manager.cpp user_dao.cpp book_dao.cpp borrow_record_dao.cpp
    category_dao.cpp log_dao.cpp reservation_dao.cpp
    auth_service.cpp book_service.cpp borrow_service.cpp reservation_service.cpp
    log_service.cpp overdue_scanner.cpp utils.cpp api_bridge.cpp
    search_index.cpp recommend_engine.cpp fine_calculator.cpp
    category_tree.cpp reservation_queue.cpp credit_engine.cpp
    csv_exporter.cpp json_exporter.cpp
    status_code.cpp entity_ctors.cpp)
target_link_libraries(library_core PRIVATE ${SQLite3_LIBRARIES})

# Test executables
set(TEST_SRCS
    test_credit_engine test_fine_calculator test_search_index
    test_recommend_engine test_reservation_queue test_borrow_service)
foreach(t ${TEST_SRCS})
    add_executable(${t} ${t}.cpp)
    target_link_libraries(${t} PRIVATE library_core ${SQLite3_LIBRARIES})
endforeach()

add_executable(mock_generator mock_generator.cpp)
target_link_libraries(mock_generator PRIVATE library_core ${SQLite3_LIBRARIES})

add_executable(performance_test performance_test.cpp)
target_link_libraries(performance_test PRIVATE library_core ${SQLite3_LIBRARIES})
