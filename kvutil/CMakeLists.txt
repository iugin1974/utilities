cmake_minimum_required(VERSION 3.14)
project(kvutil
    VERSION 1.0.0
    DESCRIPTION "Utility header-only per la lettura di file chiave=valore"
    LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Libreria header-only: non produce un binario, esporta solo l'header
add_library(kvutil INTERFACE)
add_library(kvutil::kvutil ALIAS kvutil)
target_include_directories(kvutil INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(kvutil INTERFACE cxx_std_17)
# --- Installazione (opzionale, utile se kvutil viene distribuito) ---
include(GNUInstallDirs)
install(FILES src/value_reader.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(TARGETS kvutil
    EXPORT kvutilTargets
)
install(EXPORT kvutilTargets
    FILE kvutilTargets.cmake
    NAMESPACE kvutil::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/kvutil
)
