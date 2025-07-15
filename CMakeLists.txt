cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME rc_ecat_zgt6_hw)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")
include("cmake/cmake_func.cmake") # 工具函数

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)
add_subdirectory(third_party/SOES)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE

    # Add user defined library search paths
)

file(GLOB_RECURSE SOURCES
    "Drivers/*.c"
    "App/*.c"
    "Bsp/*.c"
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE

    # Add user sources here
    ${SOURCES}
)

include_sub_directories_recursively(${CMAKE_SOURCE_DIR}/App)
include_sub_directories_recursively(${CMAKE_SOURCE_DIR}/Bsp)
include_sub_directories_recursively(${CMAKE_SOURCE_DIR}/Core)
include_sub_directories_recursively(${CMAKE_SOURCE_DIR}/Drivers)
include_sub_directories_recursively(${CMAKE_SOURCE_DIR}/Middlewares)
include_sub_directories_recursively(${CMAKE_SOURCE_DIR}/third_party)
include_sub_directories_recursively(${CMAKE_SOURCE_DIR}/USB_DEVICE)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE

    # Add user defined include paths
)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE DEBUG)

# Debug 模式下可以不做优化或开启额外的调试信息
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE NDEBUG)

    # Release 模式下开启优化选项（未写）
endif()

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE

    LAN9252

    # ONLY_CAN0
    DOIT_INLOOP

    # BACKEND_PROCESS_CANDATA # 经测试之后can中断过于频繁，while被频频打断，无法正常处理数据

    # Add user defined symbols
    # use these symbols to choose which mode:FreeRun/Mixed Mode/Interrupt Mode
    # FreeRun
    MixedMode

    # InterruptMode

    # 测试舵轮从站控制板固件
    TEST_SWERVE

    # 测试can发送任务队列
    IF_USE_SENDQUEUE

    # 使用LL库作为HAL库的底层驱动
    USE_LL_DRIVER

    # 使用DMA的SPI
    # USE_DMA_SPI

    # 试一下用can tx发送空闲中断来逼近理论极限
    # USE_CAN_TX_IDLE_INTERRUPT

    # 测试耗时的程序，可以关了
    # TEST_TIME
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
    soes
    soes_esi
)

# include transfer tasks after build (可得到.hex)
include("cmake/post_build_tasks.cmake")

# 添加jlink下载烧录项
add_custom_target(download_jlink
    COMMAND JFlash -openprj"${CMAKE_SOURCE_DIR}/rc_ecat_zgt6_hw.jflash" -open"${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.hex",0x08000000 -auto -startapp -exit
    DEPENDS ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.hex
    COMMENT "Download ${CMAKE_PROJECT_NAME}.hex to target device"
)