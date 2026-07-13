cmake_minimum_required(VERSION 3.16)
project(configutil LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(configutil STATIC
    src/appconfig.cpp
)

target_include_directories(configutil PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

option(CONFIGUTIL_BUILD_EXAMPLE "Compila il programma di esempio" ON)
if(CONFIGUTIL_BUILD_EXAMPLE)
    add_executable(configutil_example example/main.cpp)
    target_link_libraries(configutil_example PRIVATE configutil)
endif()
