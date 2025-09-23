cmake_minimum_required(VERSION 3.16)

# 设置项目语言（明确指定只使用C++）
project(TimeArtifacts VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置构建类型（如果未指定）
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 编译选项
if(MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# 查找依赖包
find_package(nlohmann_json CONFIG QUIET)
find_package(Threads REQUIRED)

# 如果找不到nlohmann_json，尝试使用系统路径
if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann_json not found via CONFIG, trying to find manually...")
    find_path(NLOHMANN_JSON_INCLUDE_DIR nlohmann/json.hpp)
    if(NLOHMANN_JSON_INCLUDE_DIR)
        add_library(nlohmann_json::nlohmann_json INTERFACE IMPORTED)
        target_include_directories(nlohmann_json::nlohmann_json INTERFACE ${NLOHMANN_JSON_INCLUDE_DIR})
        message(STATUS "Found nlohmann_json at: ${NLOHMANN_JSON_INCLUDE_DIR}")
    else()
        message(WARNING "nlohmann_json not found! Using fallback JSON handling.")
    endif()
endif()

# websocketpp是header-only库，直接包含头文件即可
# SFML音频库（音频工程师负责集成）
# find_package(sfml COMPONENTS audio CONFIG REQUIRED)

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 收集源文件
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/*.h"
)

file(GLOB_RECURSE HEADERS 
    "include/*.h"
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# 链接库
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
    Threads::Threads
)

# 如果找到了nlohmann_json，则链接它
if(TARGET nlohmann_json::nlohmann_json)
    target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
    message(STATUS "Linking with nlohmann_json")
else()
    message(STATUS "Building without nlohmann_json - using fallback")
endif()

# sfml-audio  # 音频工程师添加

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 复制共享资源到构建目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/../shared/data $<TARGET_FILE_DIR:${PROJECT_NAME}>/data
)

# 开发者选项
option(BUILD_TESTS "Build unit tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# 输出构建信息
message(STATUS "时光信物后端项目配置完成")
message(STATUS "构建类型: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++标准: ${CMAKE_CXX_STANDARD}")
message(STATUS "编译器: ${CMAKE_CXX_COMPILER_ID}")
