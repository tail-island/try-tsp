cmake_minimum_required(VERSION 3.19)

project(two-opt LANGUAGES CUDA CXX)

find_package(CUDAToolkit REQUIRED)
find_package(Thrust      REQUIRED)

thrust_create_target(Thrust)

add_executable(${PROJECT_NAME}
    io.hpp
    main.cu
    solver.hpp
)

target_compile_options(${PROJECT_NAME} PRIVATE
    --std=c++20
    --extended-lambda
    --expt-relaxed-constexpr
)

target_compile_features(${PROJECT_NAME} PRIVATE
    cxx_std_20
    cuda_std_11  # cuda_std_11にすると-stdが生成されなくなるので、上で定義した--std=c++20が有効になる。ダーティ・ハックでごめんなさい。。。
)

target_link_libraries(${PROJECT_NAME}
    CUDA::cudart
    Thrust
)
