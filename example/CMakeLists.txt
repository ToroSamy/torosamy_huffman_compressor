cmake_minimum_required(VERSION 3.10)
project(example)

# 使用 C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_executable(${PROJECT_NAME} main.cpp)


target_link_libraries(${PROJECT_NAME}
        /opt/torosamy_huffman_compressor/lib/libtorosamy_huffman_compressor.so
)

target_include_directories(${PROJECT_NAME} PUBLIC
        /opt/torosamy_huffman_compressor/include
)