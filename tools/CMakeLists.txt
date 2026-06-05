cmake_minimum_required(VERSION 3.25)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

project(reset)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(WIN32)

add_executable(echo
#	enumvolumes.cpp
	enumports.cpp
	echocoms.cpp
)
target_include_directories(echo PRIVATE .)
target_link_libraries(echo PRIVATE setupapi)

add_executable(rpc
	enumports.cpp
	rpccoms.cpp
)
target_include_directories(rpc PRIVATE .)
target_link_libraries(rpc PRIVATE setupapi)

endif()


if(WIN32)
add_executable(reset
#	enumvolumes.cpp
	enumports.cpp
	resetcoms.cpp
)
target_include_directories(reset PRIVATE .)
target_link_libraries(reset PRIVATE setupapi)
endif()

if(LINUX)
add_executable(reset
	enumusb.cpp
)

target_link_libraries(reset
	crypto
	udev
	asound
	m
	pthread
	rt
	dl
	drm
#	xml2
	hiredis
#	-static-libstdc++ 
#	-static-libgcc
)

endif()

