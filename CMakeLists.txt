cmake_minimum_required(VERSION 3.18)
project(libaudio)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 20)

include(ThisProject)

add_subdirectory(src)

if (THIS_PROJECT)
    set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Werror -O3")

    if (BUILD_TESTING)
        enable_testing()
        add_subdirectory(tests)
    endif ()
endif ()
