# project properties
cmake_minimum_required(VERSION 3.10.2)
project(BKGraph LANGUAGES CXX)
if(CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# source files
set(SRC_FILES
	"src/engine/drawSys.cpp"
	"src/engine/drawSys.h"
	"src/engine/filer.cpp"
	"src/engine/filer.h"
	"src/engine/scene.cpp"
	"src/engine/scene.h"
	"src/engine/windowSys.cpp"
	"src/engine/windowSys.h"
	"src/engine/world.cpp"
	"src/engine/world.h"
	"src/prog/defaults.h"
	"src/prog/program.cpp"
	"src/prog/program.h"
	"src/prog/progs.cpp"
	"src/prog/progs.h"
	"src/utils/functions.cpp"
	"src/utils/functions.h"
	"src/utils/parser.cpp"
	"src/utils/parser.h"
	"src/utils/utils.cpp"
	"src/utils/utils.h"
	"src/utils/vec2.h"
	"src/widgets/context.cpp"
	"src/widgets/context.h"
	"src/widgets/graphView.cpp"
	"src/widgets/graphView.h"
	"src/widgets/layouts.cpp"
	"src/widgets/layouts.h"
	"src/widgets/widgets.cpp"
	"src/widgets/widgets.h")

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	list(APPEND SRC_FILES "rsc/resource.rc")
endif()

# dependencies
set(VER_SDL "2.0.12" CACHE STRING "SDL2 version.")
set(VER_TTF "2.0.15" CACHE STRING "SDL2_ttf version.")
set(DIR_LIB "${CMAKE_SOURCE_DIR}/lib")
set(DIR_RSC "${CMAKE_SOURCE_DIR}/rsc")

function(downloadLib URL DST LIBDIR)
	get_filename_component(LIBDIR "${LIBDIR}" NAME)
	if(EXISTS "${DST}/${LIBDIR}")
		message(STATUS "Found ${DST}/${LIBDIR}")
		return()
	endif()

	get_filename_component(FILE "${URL}" NAME)
	if(EXISTS "${DST}/${FILE}")
		message(STATUS "Found ${DST}/${FILE}")
	else()
		message(STATUS "Downloading ${URL}")
		file(DOWNLOAD "${URL}" "${DST}/${FILE}")
	endif()

	if(FILE MATCHES ".zip$")
		execute_process(COMMAND "${CMAKE_COMMAND}" -E tar xf "${FILE}" WORKING_DIRECTORY "${DST}")
	elseif(FILE MATCHES ".tar.gz$")
		execute_process(COMMAND "${CMAKE_COMMAND}" -E tar xzf "${FILE}" WORKING_DIRECTORY "${DST}")
	endif()
endfunction()

file(MAKE_DIRECTORY "${DIR_LIB}")
include_directories("${CMAKE_SOURCE_DIR}/src")
if(MSVC)
	set(DIR_SDL "${DIR_LIB}/SDL2-${VER_SDL}")
	set(DIR_TTF "${DIR_LIB}/SDL2_ttf-${VER_TTF}")
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(BIN_SDL "${DIR_SDL}/lib/x64")
		set(BIN_TTF "${DIR_TTF}/lib/x64")
	else()
		set(BIN_SDL "${DIR_SDL}/lib/x86")
		set(BIN_TTF "${DIR_TTF}/lib/x86")
	endif()
	set(LIB_SDL "${BIN_SDL}")
	set(LIB_TTF "${BIN_TTF}")
	set(INC_SDL "${DIR_SDL}/include")
	set(INC_TTF "${DIR_TTF}/include")
	set(URL_SDL "https://www.libsdl.org/release/SDL2-devel-${VER_SDL}-VC.zip")
	set(URL_TTF "https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-devel-${VER_TTF}-VC.zip")

	downloadLib("${URL_SDL}" "${DIR_LIB}" "${DIR_SDL}")
	downloadLib("${URL_TTF}" "${DIR_LIB}" "${DIR_TTF}")

	link_directories("${LIB_SDL}" "${LIB_TTF}")
	include_directories("${INC_SDL}" "${INC_TTF}")
endif()

# compiler/linker flags
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	add_definitions(-D_UNICODE -D_CRT_SECURE_NO_WARNINGS)
endif()

if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
	set(CXX_FLG_ANY "-Wall -Wextra -pedantic-errors -Wdouble-promotion -Wformat=2 -Wmissing-declarations -Wold-style-cast -Wshadow -Wunreachable-code -Wno-implicit-fallthrough -Wno-parentheses -Wno-switch")
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set(CXX_FLG_ANY "${CXX_FLG_ANY} -Wcast-align=strict -Wduplicated-branches -Wduplicated-cond -Wlogical-op -Wuseless-cast")
	else()
		set(CXX_FLG_ANY "${CXX_FLG_ANY} -Wcast-align -Wnull-dereference -Wshadow-all -Wno-unused-private-field")
	endif()

	set(FLG_DEBUG "-g")
	set(FLG_RELEASE "-O3")
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set(FLG_RELEASE "${FLG_RELEASE} -s")
	endif()
elseif(MSVC)
	set(CXX_FLG_ANY "/W4 /wd4100 /wd4201 /wd4244 /wd4458 /wd4706 /wd4805")
	set(FLG_DEBUG "/Od /Ob0")
	set(FLG_RELEASE "/O2 /Ob2 /Oi /Ot /GL")
endif()

if(CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CXX_FLG_ANY} ${FLG_DEBUG}")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CXX_FLG_ANY} ${FLG_RELEASE}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLG_ANY} ${FLG_DEBUG}")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLG_ANY} ${FLG_RELEASE}")
endif()

# set main target
add_executable(BKGraph ${SRC_FILES})
target_link_libraries(BKGraph SDL2 SDL2_ttf)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set_target_properties(BKGraph PROPERTIES
						RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
						RUNTIME_OUTPUT_DIRECTORY_DEBUG "${EXECUTABLE_OUTPUT_PATH}"
						RUNTIME_OUTPUT_DIRECTORY_RELEASE "${EXECUTABLE_OUTPUT_PATH}")

# post buils commands
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	add_custom_command(TARGET BKGraph POST_BUILD
						COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${BIN_SDL}/SDL2.dll" "${BIN_TTF}/SDL2_ttf.dll" "${BIN_TTF}/libfreetype-6.dll" "${BIN_TTF}/zlib1.dll" "${EXECUTABLE_OUTPUT_PATH}"
						COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${DIR_SDL}/COPYING.txt" "${EXECUTABLE_OUTPUT_PATH}/COPYING.SDL.txt"
						COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${DIR_TTF}/COPYING.txt" "${EXECUTABLE_OUTPUT_PATH}/COPYING.SDL_ttf.txt"
						COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${BIN_TTF}/LICENSE.freetype.txt" "${EXECUTABLE_OUTPUT_PATH}"
						COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${BIN_TTF}/LICENSE.zlib.txt" "${EXECUTABLE_OUTPUT_PATH}")
else()
	add_custom_command(TARGET BKGraph POST_BUILD
						COMMAND chmod +x "${DIR_RSC}/run.sh"
						COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${DIR_RSC}/run.sh" "${EXECUTABLE_OUTPUT_PATH}"
						COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${DIR_RSC}/bkgraph.desktop" "${EXECUTABLE_OUTPUT_PATH}")
endif()
add_custom_command(TARGET BKGraph POST_BUILD
					COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${DIR_RSC}/icon.bmp" "${EXECUTABLE_OUTPUT_PATH}")

# file filter
foreach(FSRC IN LISTS SRC_FILES)
	get_filename_component(FGRP "${FSRC}" DIRECTORY)
	string(REPLACE "/" ";" FGRP "${FGRP}")
	list(REMOVE_AT FGRP 0)
	if(MSVC)
		string(REPLACE ";" "\\" FGRP "${FGRP}")
	else()
		string(REPLACE ";" "/" FGRP "${FGRP}")
	endif()
	source_group("${FGRP}" FILES "${FSRC}")
endforeach()
