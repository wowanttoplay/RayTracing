cmake_minimum_required(VERSION 3.17)
project(RayTracing VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xclang -fconcepts-ts")

# 将当前目录下的源文件包含入工程中编译
aux_source_directory(. DIR_SRC)
add_executable(RayTracing ${DIR_SRC})

