set(CMAKE_PROJECT_MINIMUM_VERSION 3.20)
cmake_minimum_required(VERSION 3.20)

project(fipper
	VERSION 0.0.0
	LANGUAGES CXX
)

set(PROJECT_ACRONYM fp)
string(TOUPPER ${PROJECT_ACRONYM} PROJECT_ACRONYM_UPPERCASE)
set(PROJECT_CPP_DIALECT 23)

option(BUILD_TESTS "BUILD_TESTS" OFF)

add_compile_options($<$<CONFIG:Debug>:-fsanitize=address>)
add_link_options($<$<CONFIG:Debug>:-fsanitize=address>)

add_subdirectory(lib)
add_subdirectory(sandbox)
add_subdirectory(loader)
if (BUILD_TESTS)
	add_subdirectory(tests)
endif()
