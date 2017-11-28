cmake_minimum_required(VERSION 3.1.3)
project(BKGraph LANGUAGES CXX)
set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")

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
	"src/prog/program.cpp"
	"src/prog/program.h"
	"src/prog/progs.cpp"
	"src/prog/progs.h"
	"src/utils/defaults.h"
	"src/utils/parser.cpp"
	"src/utils/parser.h"
	"src/utils/settings.cpp"
	"src/utils/settings.h"
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
	
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	list(APPEND SRC_FILES "rsc/resource.rc")
endif()

# include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()

# library/framework directories
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	if (MSVC)
		if (${CMAKE_SIZEOF_VOID_P} MATCHES "8")
			set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib/msvc64)
		else()
			set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib/msvc32)
		endif()	
	endif()
    
	link_directories(${LIB_DIR})
	set(DLL_FILES
		"libfreetype-6.dll"
		"SDL2.dll"
		"SDL2_ttf.dll"
		"zlib1.dll")
endif()

# definitions
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_definitions(-D_UNICODE -D_CRT_SECURE_NO_WARNINGS)
endif()
	
# set main target
add_executable(BKGraph ${SRC_FILES})
target_compile_features(BKGraph PUBLIC cxx_nullptr cxx_enum_forward_declarations cxx_variadic_templates cxx_alias_templates cxx_unicode_literals cxx_long_long_type cxx_rvalue_references)

# linker flags
target_link_libraries(BKGraph SDL2 SDL2_ttf)

# target path
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

# target properties
set_target_properties(BKGraph PROPERTIES
									OUTPUT_NAME BKGraph
									RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
									RUNTIME_OUTPUT_DIRECTORY_DEBUG "${EXECUTABLE_OUTPUT_PATH}"
									RUNTIME_OUTPUT_DIRECTORY_RELEASE "${EXECUTABLE_OUTPUT_PATH}")
								
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set_target_properties(BKGraph PROPERTIES
									LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
									LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
endif()

# uncomment the following line for 32 bit build (unless the compiler doesn't use the "-m32" option)
# set_target_properties(BKGraph PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")

# post buils commands
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	foreach(dll_file ${DLL_FILES})
		add_custom_command(TARGET BKGraph POST_BUILD
							COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LIB_DIR}/${dll_file} ${EXECUTABLE_OUTPUT_PATH})
	endforeach()
endif()
add_custom_command(TARGET BKGraph POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/rsc/BKGraph.ico ${EXECUTABLE_OUTPUT_PATH})
					
# filter file for ms visual studio
foreach(source IN LISTS SRC_FILES)
	get_filename_component(source_dir ${source} PATH)
	string(REPLACE "/" ";" dirs "${source_dir}")
	list(GET dirs 0 dir0)
	
	if (${dir0} MATCHES "rsc")
		set(filter_path "Source")
	else()
		list(REMOVE_AT dirs 0)
		list(INSERT dirs 0 "Source")
		string(REPLACE ";" "\\" filter_path "${dirs}")
	endif()
	source_group("${filter_path}" FILES ${source})
endforeach()

