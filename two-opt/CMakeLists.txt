cmake_minimum_required(VERSION 3.18)

project(two-opt LANGUAGES CXX)

add_executable(${PROJECT_NAME}
    io.hpp
    main.cpp
    solver.hpp
)

target_compile_features(${PROJECT_NAME} PRIVATE
    cxx_std_20
)
