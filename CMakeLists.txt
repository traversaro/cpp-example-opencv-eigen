cmake_minimum_required(VERSION 3.16...3.27)

project(rerun_external_cpp_proj LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

option(USE_RERUN_VIA_FIND_PACKAGE "Use find_package to find rerun" OFF)

if(NOT USE_RERUN_VIA_FIND_PACKAGE)
  include(FetchContent)
  FetchContent_Declare(rerun_sdk URL https://github.com/rerun-io/rerun/releases/download/0.10.0/rerun_cpp_sdk.zip)
  FetchContent_MakeAvailable(rerun_sdk)
  if(NOT TARGET rerun_sdk::rerun_sdk)
    add_library(rerun_sdk::rerun_sdk ALIAS rerun_sdk)
  endif()
else()
  find_package(rerun_sdk REQUIRED)
endif()

find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(rerun_ext_example src/main.cpp)

target_link_libraries(rerun_ext_example Eigen3::Eigen ${OpenCV_LIBS} rerun_sdk::rerun_sdk)
