cmake_minimum_required(VERSION 4.3)
project(reset)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(reset
#	enumvolumes.cpp
	enumports.cpp
	resetcoms.cpp
)

target_include_directories(reset PRIVATE .)
target_link_libraries(reset PRIVATE setupapi)
