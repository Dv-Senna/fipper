cmake_minimum_required(VERSION 3.20)


set(LOADER_NAME ${PROJECT_NAME}_loader)
set(LOADER_VERSION ${PROJECT_VERSION})
set(LOADER_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(LOADER_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(LOADER_VERSION_PATCH ${PROJECT_VERSION_PATCH})
string(TOUPPER ${LOADER_NAME} LOADER_NAME_UPPERCASE)

add_compile_definitions(
	${LOADER_NAME_UPPERCASE}_NAME=${LOADER_NAME}
	${LOADER_NAME_UPPERCASE}_VERSION_MAJOR=${LOADER_VERSION_MAJOR}
	${LOADER_NAME_UPPERCASE}_VERSION_MINOR=${LOADER_VERSION_MINOR}
	${LOADER_NAME_UPPERCASE}_VERSION_PATCH=${LOADER_VERSION_PATCH}
)

if (BUILD_STATIC)
	add_compile_definitions(${PROJECT_ACRONYM_UPPERCASE}_STATIC)
else()
	add_compile_definitions(${PROJECT_ACRONYM_UPPERCASE}_SHARED)
endif()

include_directories(
	./include,
	../lib/include
)

file(GLOB_RECURSE LOADER_HEADER *.hpp)
file(GLOB_RECURSE LOADER_INLINE *.inl)
file(GLOB_RECURSE LOADER_SOURCE *.cpp)

add_executable(${LOADER_NAME} ${LOADER_HEADER} ${LOADER_INLINE} ${LOADER_SOURCE})
set_property(TARGET ${LOADER_NAME} PROPERTY CXX_STANDARD ${PROJECT_CPP_DIALECT})

#target_link_directories(./lib)

target_link_libraries(
	${LOADER_NAME} PRIVATE ${PROJECT_NAME}
)

if (MSVC)
	target_compile_options(${LOADER_NAME} PRIVATE /W4)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(${LOADER_NAME} PRIVATE -Wall -Wextra -pedantic)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	target_compile_options(${LOADER_NAME} PRIVATE -Wall -Wextra -pedantic -stdlib=libc++)
endif()


if (WIN32)
	add_compile_definitions(${PROJECT_ACRONYM_UPPERCASE}_WINDOWS)
	add_compile_definitions(${LOADER_NAME_UPPERCASE}_WINDOWS)
	if (BUILD_STATIC)
	else()
		message("Copying dll")
		message("${PROJECT_BINARY_DIR}/lib/lib${PROJECT_NAME}.dll")
		message("${PROJECT_BINARY_DIR}/loader")
		add_custom_command(
			TARGET ${LOADER_NAME}
			PRE_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${PROJECT_BINARY_DIR}/lib/lib${PROJECT_NAME}.dll"
			"${PROJECT_BINARY_DIR}/loader"
		)
	endif()
elseif (UNIX)
	add_compile_definitions(${PROJECT_ACRONYM_UPPERCASE}_LINUX)
	add_compile_definitions(${LOADER_NAME_UPPERCASE}_LINUX)
elseif (APPLE)
	add_compile_definitions(${PROJECT_ACRONYM_UPPERCASE}_APPLE)
	add_compile_definitions(${LOADER_NAME_UPPERCASE}_APPLE)
else()
	message(FATAL_ERROR "Platforms other than Windows, Linux and MacOS aren't supported for now")
endif()
