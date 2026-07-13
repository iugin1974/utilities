cmake_minimum_required(VERSION 3.16)
project(ftputil LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)

# --- Libreria ---
add_library(ftputil STATIC
    src/ftp.cpp
)

target_include_directories(ftputil PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ftputil PUBLIC CURL::libcurl)

# --- Eseguibile di esempio (facoltativo, per testare la libreria) ---
option(FTPUTIL_BUILD_EXAMPLE "Compila il programma di esempio" ON)
if(FTPUTIL_BUILD_EXAMPLE)
    add_executable(ftputil_example example/main.cpp)
    target_link_libraries(ftputil_example PRIVATE ftputil)
endif()
