cmake_minimum_required(VERSION 3.22)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

# 添加 soes 子目录，soes 库的定义在soes/目录下
add_subdirectory(soes)
add_library(soes_esi STATIC
  soes-esi/objectlist.c
)
target_include_directories(soes_esi PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/soes-esi
  ${CMAKE_CURRENT_SOURCE_DIR}/soes
)
target_link_libraries(soes PUBLIC soes_esi)