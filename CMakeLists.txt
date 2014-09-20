cmake_minimum_required(VERSION 2.6)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: Debug, Release, MinSizeRel, RelWithDebInfo." FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

project(hide_and_kill)

set(CMAKE_MODULE_PATH cmake/Modules)

if(${CMAKE_BUILD_TYPE} MATCHES Release)
	set(EXECUTABLE_OUTPUT_PATH .)
else()
	set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++11")
endif()

file(GLOB_RECURSE source_files src/*.cpp src/*.h src/*.hpp src/*.inl)
include_directories(src src/application src/client src/common src/game src/graphics src/gui src/server)

find_package(SFML 2.1 COMPONENTS SYSTEM WINDOW NETWORK GRAPHICS REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${SFML_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
link_directories(${SFML_LIBRARY_DIR})

add_executable(hide_and_kill ${source_files})

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
	target_link_libraries(hide_and_kill ${SFML_SYSTEM_LIBRARY_DEBUG} ${SFML_WINDOW_LIBRARY_DEBUG} ${SFML_NETWORK_LIBRARY_DEBUG} ${SFML_GRAPHICS_LIBRARY_DEBUG} ${OPENGL_gl_LIBRARY})
else()
	target_link_libraries(hide_and_kill ${SFML_SYSTEM_LIBRARY_RELEASE} ${SFML_WINDOW_LIBRARY_RELEASE} ${SFML_NETWORK_LIBRARY_RELEASE} ${SFML_GRAPHICS_LIBRARY_RELEASE} ${OPENGL_gl_LIBRARY})
endif()

