cmake_minimum_required(VERSION 3.21)
project(insaneSums)

# compiler settings
if(WIN32)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /W4")
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(WIN32)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include Conan build files
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)
set(CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

# global include settings
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${CMAKE_BINARY_DIR})
include_directories(${CONAN_INCLUDE_DIRS_GSL_MICROSOFT})

# Qt6
find_package(Qt6 COMPONENTS Core Widgets REQUIRED CONFIG)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC OFF)

#find_package(Qt6 COMPONENTS
#	Widgets REQUIRED
#)

# Source
file(GLOB_RECURSE headers src/*.h)
file(GLOB_RECURSE sources src/*.cpp)
file(GLOB_RECURSE qtforms res/forms/*.ui)
set(qtres res/res.qrc)

source_group(
	TREE ${CMAKE_CURRENT_SOURCE_DIR}
	PREFIX src
	FILES ${headers} ${sources}
)

qt6_wrap_ui(qtforms_generated ${qtforms})

if(WIN32)
	set(GUI_TYPE WIN32)
	#list(APPEND changes_SOURCES changes.rc)
endif(WIN32)
if(APPLE)
	set(GUI_TYPE MACOSX_BUNDLE)
endif(APPLE)

# Target
add_executable(${PROJECT_NAME}
	${GUI_TYPE}
	${headers}
	${sources}
	${qtforms}
	${qtforms_generated}
	${qtres}
	res/res.rc
)

target_compile_definitions(${PROJECT_NAME}
	PUBLIC CRYPTOPP_ENABLE_NAMESPACE_WEAK=1
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE Qt6::Widgets
	PRIVATE CONAN_PKG::cryptopp
)

#if(WIN32)
#	target_link_libraries(${PROJECT_NAME}
#		PRIVATE Ws2_32.lib
#	)
#endif(WIN32)
#
#if(UNIX)
#	target_link_libraries(${PROJECT_NAME}
#		PRIVATE pthread
#	)
#endif(UNIX)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/res/qt.conf" $<TARGET_FILE_DIR:${PROJECT_NAME}>)
