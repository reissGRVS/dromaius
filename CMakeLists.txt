cmake_minimum_required(VERSION 3.4)

project(Dromaius)
# The version number.
set (Dromaius_VERSION_MAJOR 0)
set (Dromaius_VERSION_MINOR 1)

set (CPP_SRC ${CMAKE_SOURCE_DIR}/src)
set (CPP_INC ${CMAKE_SOURCE_DIR}/include)

# Substitute our version numbers to main.cpp
configure_file(
	"${CPP_SRC}/main.cpp.in"
	"${CPP_SRC}/main.cpp"
)

add_definitions(-std=c++14 -Wall -Wextra -fmax-errors=10 -pedantic -g)

include_directories("${CPP_INC}")
add_executable(
	Dromaius 
	${CPP_SRC}/main.cpp
	${CPP_SRC}/memorymap.cpp
	${CPP_SRC}/cpu.cpp
	${CPP_SRC}/gpu.cpp
	${CPP_SRC}/timer.cpp
	${CPP_SRC}/joypad.cpp
	${CPP_SRC}/gameboy.cpp
	${CPP_SRC}/opcodes/opcodes.cpp
	${CPP_SRC}/opcodes/cbopcodes.cpp
	${CPP_SRC}/opcodes/genericops.cpp
)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED graphics window system audio)
include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(Dromaius ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})

install(TARGETS Dromaius DESTINATION bin)