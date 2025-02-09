set(CMAKE_PROJECT_MINIMUM_VERSION 3.20)
cmake_minimum_required(VERSION 3.20)

project(fipper
	VERSION 0.0.0
	LANGUAGES CXX
)

set(PROJECT_ACRONYM fp)
string(TOUPPER ${PROJECT_ACRONYM} PROJECT_ACRONYM_UPPERCASE)
set(PROJECT_CPP_DIALECT 23)

set(FIPPER_REFLECTION_STRUCTURE_MAX_MEMBERS_COUNT 64)
set(FIPPER_VAR_TO_SEQ_MAX_SIZE ${FIPPER_REFLECTION_STRUCTURE_MAX_MEMBERS_COUNT})

option(BUILD_TESTS "BUILD_TESTS" OFF)

add_subdirectory(vendors/glaze)

set(PROJECT_DEPS_LIBS glaze::glaze)
set(PROJECT_SCRIPTS ${PROJECT_SOURCE_DIR}/scripts)
set(PROJECT_PYTHON_SCRIPTS ${PROJECT_SCRIPTS}/python)
set(PROJECT_GENERATOR_SCRIPTS ${PROJECT_PYTHON_SCRIPTS}/generator)

add_compile_options($<$<CONFIG:Debug>:-fsanitize=address>)
add_link_options($<$<CONFIG:Debug>:-fsanitize=address>)

add_subdirectory(lib)
add_subdirectory(sandbox)
if (BUILD_TESTS)
	add_subdirectory(tests)
endif()
