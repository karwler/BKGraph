# project properties
cmake_minimum_required(VERSION 3.1.3)
project(BKGraph LANGUAGES CXX)
set(CMAKE_BUILD_TYPE "Release")
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
	"src/utils/functions.cpp"
	"src/utils/functions.h"
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

# library directories and dlls
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
		"libpng16-16.dll"
		"SDL2.dll"
		"SDL2_image.dll"
		"SDL2_ttf.dll"
		"zlib1.dll")
endif()

# definitions
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_definitions(-D_UNICODE -D_CRT_SECURE_NO_WARNINGS)
endif()

# set main target
add_executable(BKGraph ${SRC_FILES})
target_compile_features(BKGraph PUBLIC cxx_aggregate_default_initializers cxx_alias_templates cxx_alignas cxx_alignof cxx_attributes cxx_constexpr cxx_contextual_conversions cxx_decltype cxx_default_function_template_args cxx_defaulted_functions cxx_defaulted_move_initializers cxx_delegating_constructors cxx_deleted_functions cxx_enum_forward_declarations cxx_explicit_conversions cxx_extended_friend_declarations cxx_extern_templates cxx_final cxx_func_identifier cxx_generalized_initializers cxx_inheriting_constructors cxx_inline_namespaces cxx_local_type_template_args cxx_long_long_type cxx_noexcept cxx_nonstatic_member_init cxx_nullptr cxx_override cxx_range_for cxx_raw_string_literals cxx_reference_qualified_functions cxx_right_angle_brackets cxx_rvalue_references cxx_sizeof_member cxx_strong_enums cxx_unicode_literals cxx_uniform_initialization cxx_unrestricted_unions cxx_user_literals cxx_variadic_macros cxx_variadic_templates cxx_template_template_parameters)

# uncomment the following line for 32 bit build (unless the compiler doesn't use the "-m32" option)
#set_target_properties(BKGraph PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")

# set include directories and link libraries
target_include_directories(BKGraph PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	target_include_directories(BKGraph PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()
target_link_libraries(BKGraph SDL2 SDL2_image SDL2_ttf)

# target properties
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
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

# post buils commands
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	foreach(dll_file ${DLL_FILES})
		add_custom_command(TARGET BKGraph POST_BUILD
							COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LIB_DIR}/${dll_file} ${EXECUTABLE_OUTPUT_PATH})
	endforeach()
else()
	add_custom_command(TARGET BKGraph POST_BUILD
					COMMAND chmod +x ${CMAKE_SOURCE_DIR}/rsc/run.sh
					COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/rsc/run.sh ${EXECUTABLE_OUTPUT_PATH})
endif()
add_custom_command(TARGET BKGraph POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/rsc/icon.png ${EXECUTABLE_OUTPUT_PATH})

# file filter
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

