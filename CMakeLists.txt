cmake_minimum_required(VERSION 3.0.0)
project(npl VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD_REQUIRED 20)
set(CMAKE_CXX_STANDARD 20)

include_directories(lib/libnpl)

add_executable(main src/main.cpp)

include(CTest)
enable_testing()


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
