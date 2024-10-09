cmake_minimum_required(VERSION 3.24.0)

# For Windows
if (WIN32)
    message(STATUS windows system)
    # Qt5 config路径
    set(CMAKE_PREFIX_PATH  D:/Qt/5.15.2/msvc2019_64/lib/cmake/Qt5)
    # 找x64库
#        set(VCPKG_TARGET_TRIPLET "x64-windows")
    #    set(VCPKG_ROOT "$ENV{USERPROFILE}/vcpkg")
    set(VCPKG_HOME "$ENV{USERPROFILE}/vcpkg")
    file(TO_CMAKE_PATH "${VCPKG_HOME}" VCPKG_HOME)
    add_definitions(-D_WIN32_WINNT=0x600)
endif()

# For Linux
if (UNIX)
    set(VCPKG_HOME "$ENV{HOME}/vcpkg")
    set(CMAKE_PREFIX_PATH  $ENV{HOME}/Qt/5.15.2/gcc_64/lib/cmake/Qt5)
endif()

# For macOS
if (APPLE)
    set(VCPKG_HOME "$ENV{HOME}/vcpkg")
    set(CMAKE_PREFIX_PATH  $ENV{HOME}/Qt/5.15.2/clang_64/lib/cmake/Qt5)
endif()

# 首选C++17，没有会使用上一个版本off
set(CMAKE_CXX_STANDARD 17 REQUIRED)

set (CMAKE_TOOLCHAIN_FILE "${VCPKG_HOME}/scripts/buildsystems/vcpkg.cmake")
message(CMAKE_TOOLCHAIN_FILE =${CMAKE_TOOLCHAIN_FILE})

project(FPGA_Command LANGUAGES CXX)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)
#
#find_package(Qt5  REQUIRED COMPONENTS Core Network)

find_package(PkgConfig REQUIRED)
pkg_check_modules(libusb REQUIRED IMPORTED_TARGET libusb-1.0)


find_package(OpenCV CONFIG REQUIRED)

#find_package(nlohmann_json CONFIG REQUIRED)

# boost pfr只有头文件
include_directories(${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)


#message(STATUS Boost_INCLUDE_DIRS=${Boost_INCLUDE_DIRS})
#include_directories(${Boost_INCLUDE_DIRS})

add_executable(control main.cpp
    CommandContent.h CommandContent.cpp
    CommandRegister.h
    FpgaCommand.h FpgaCommand.cpp
    DeviceControl.h DeviceControl.cpp
    common.h
    common.cpp
    Types.h)


#target_link_libraries(control
#        Qt5::Core
#)
## 包含libusb头文件
#target_include_directories(control PRIVATE ${LIBUSB_INCLUDE_DIRS})
## 链接libusb
#target_link_libraries(control ${LIBUSB_LIBRARIES})
target_link_libraries(control PRIVATE PkgConfig::libusb)

# 链接opencv
target_link_libraries(control PRIVATE opencv_ml opencv_dnn opencv_core opencv_flann)


#target_link_libraries(control nlohmann_json::nlohmann_json)


