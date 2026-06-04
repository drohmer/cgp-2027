cmake_minimum_required(VERSION 3.16)
project(cgp_test LANGUAGES CXX)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

# Generate main.cpp by scanning cgp/ for test_*.hpp files
set(GENERATED_MAIN "${CMAKE_CURRENT_BINARY_DIR}/main.cpp")
add_custom_command(
    OUTPUT  "${GENERATED_MAIN}"
    COMMAND ${Python3_EXECUTABLE}
            "${CMAKE_CURRENT_SOURCE_DIR}/generate_main.py"
            "${CMAKE_CURRENT_SOURCE_DIR}/../cgp"
            "${GENERATED_MAIN}"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/generate_main.py"
    COMMENT "Generating test/main.cpp"
)

# Pure-math CGP modules only (no OpenGL / GLFW dependency)
file(GLOB_RECURSE CGP_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/../cgp/01_base/*.[ch]pp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../cgp/02_numarray/*.[ch]pp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../cgp/03_files/*.[ch]pp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../cgp/04_grid_container/*.[ch]pp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../cgp/05_vec/*.[ch]pp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../cgp/06_mat/*.[ch]pp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../cgp/08_random_noise/*.[ch]pp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../cgp/09_geometric_transformation/*.[ch]pp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../cgp/10_camera_model/*.[ch]pp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../cgp/11_mesh/*.[ch]pp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../cgp/third_party/simplexnoise/*.[ch]pp"
)

add_executable(cgp_test "${GENERATED_MAIN}" ${CGP_SRC})

target_include_directories(cgp_test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
    "${CMAKE_CURRENT_SOURCE_DIR}/../cgp"
)

target_compile_definitions(cgp_test PRIVATE CGP_ERROR_EXCEPTION)

set_target_properties(cgp_test PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

if(MSVC)
    target_compile_options(cgp_test PRIVATE /W4 /permissive-
        /wd4127 /wd4267 /wd4244 /wd4996 /wd26451 /wd26495 /wd4305 /wd4458
    )
else()
    target_compile_options(cgp_test PRIVATE
        -Wall -Wextra -pedantic
        -Wfatal-errors -Wno-pragmas -Wno-unknown-warning-option
        -Wno-sign-compare -Wno-type-limits
    )
endif()
