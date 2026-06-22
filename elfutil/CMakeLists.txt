cmake_minimum_required(VERSION 3.25)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

project(elfutil)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(WIN32)

add_executable(elfutil
	elfutil.cpp
)
target_compile_definitions(elfutil PRIVATE UNICODE _UNICODE)
target_include_directories(elfutil PRIVATE .)
target_link_libraries(elfutil PRIVATE setupapi ole32)

endif()
