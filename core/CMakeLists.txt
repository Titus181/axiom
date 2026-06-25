cmake_minimum_required(VERSION 3.15)
project(AxiomCore VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 頭文件路徑
include_directories(include)

# 源文件列表
set(SOURCES
    src/rng.cpp
    src/simulator.cpp
    src/games/card_engine.cpp
    src/games/peek_baccarat.cpp
)

# 建立核心庫
add_library(axiom_core STATIC ${SOURCES})

# 測試用目標 (可選編譯為執行檔)
# add_executable(axiom_test src/main.cpp) # 未來擴充用

# pybind11 支持
find_package(pybind11 QUIET)
if(pybind11_FOUND)
    pybind11_add_module(pyaxiom binding/python_binding.cpp)
    target_link_libraries(pyaxiom PRIVATE axiom_core)
else()
    message(STATUS "pybind11 not found by CMake. Extension will be built via setup.py instead.")
endif()
