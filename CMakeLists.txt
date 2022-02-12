cmake_minimum_required(VERSION 3.10)

project(pixel_sum)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

#enable conan package manager
include(${PROJECT_SOURCE_DIR}/3rd_party/conan/conan.cmake)

#request gtest package
conan_cmake_configure(REQUIRES gtest/1.11.0 
                      GENERATORS cmake_find_package)

conan_cmake_autodetect(CONAN_SETTINGS)

#install gtest into output directory
conan_cmake_install(PATH_OR_REFERENCE ${CMAKE_BINARY_DIR}
                    BUILD missing
                    REMOTE conancenter
                    SETTINGS ${CONAN_SETTINGS})

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_compile_options("/WX" "/W4")

# add project
set(SOURCES
    "sources/pixel_sum.cpp"
    "include/pixel_sum.h")
# add sub-project
add_library(pixel_sum STATIC ${SOURCES})
target_include_directories(pixel_sum PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


find_package(gtest)

set(TEST_SOURCES
    "test/test.cpp"
    "test/support_functions.h"
    "test/support_functions.cpp"
    "test/pixel_sum_base_test.cpp"
    "test/pixel_sum_test.cpp"
    "test/pixel_average_test.cpp"
    "test/pixel_non_zero_count_test.cpp")

# add test sub-project
add_executable(pixel_sum_tests ${TEST_SOURCES})

target_link_libraries(pixel_sum_tests PUBLIC pixel_sum GTest::gtest)

add_test(NAME pixel_sum_tests 
         COMMAND pixel_sum_tests.exe)
         