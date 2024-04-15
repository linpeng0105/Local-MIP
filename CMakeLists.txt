cmake_minimum_required(VERSION 3.0)

project(Local-MIP)
set(CMAKE_CXX_STANDARD 17)

# 设置编译器优化选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")


# 设置源文件和头文件的路径

file(GLOB_RECURSE SOURCES "code/*.cpp" "code/*.cc")

set(INCLUDES ${PROJECT_SOURCE_DIR}/code)

# 生成可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 添加头文件
include_directories(${INCLUDES})

target_link_libraries(Local-MIP pthread -lpthread -lquadmath z -lz boost_thread boost_date_time boost_system)