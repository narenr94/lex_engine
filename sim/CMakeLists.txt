cmake_minimum_required(VERSION 3.10)
project(LexEngineSim VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LEXENGINE REQUIRED lexEngine)

message(STATUS "LEX_LIB_DIR = ${LEXENGINE_LIBRARY_DIRS}")

link_directories(${LEXENGINE_LIBRARY_DIRS})

add_executable(LexEngineSim sim.cpp)

target_include_directories(LexEngineSim PRIVATE ${LEXENGINE_INCLUDE_DIRS})

target_link_libraries(LexEngineSim PRIVATE ${LEXENGINE_LIBRARIES})

install(TARGETS LexEngineSim RUNTIME DESTINATION bin)