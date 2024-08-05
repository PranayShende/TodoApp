cmake_minimum_required(VERSION 3.10)

project(TODOApp)
add_executable(app main.cpp todo.cpp todo.h)
target_link_libraries(app sqlite3)
