cmake_minimum_required(VERSION 3.10)
project(ODE_02_Project LANGUAGES CXX)

set(CMAKE_OSX_SYSROOT macosx)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ODE_02 ディレクトリ内の main.cpp をビルド対象にする
add_executable(ode02_exec ODE_02/IC_solver.cpp)

# 数学関数を使う場合は math ライブラリをリンク
target_link_libraries(ode02_exec m)

# Eigenを使う場合
find_package(Eigen3 QUIET)
if(Eigen3_FOUND)
    message(STATUS "Eigen3 found")
    target_include_directories(ode02_exec PRIVATE ${EIGEN3_INCLUDE_DIR})
endif()