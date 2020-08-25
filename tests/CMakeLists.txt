include(BuildTest)
include(FetchGit)

FetchGitVersion(options "https://github.com/miniriley2012/options.git" v2)

set(CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)

option(PLAY_ENABLE_NCURSES "Enable ncurses UI for test \"play\"" ${CURSES_FOUND})

add_build_test(player player.cpp)
target_link_libraries(player "${PROJECT_NAME}" options)

if (CURSES_FOUND AND PLAY_ENABLE_NCURSES)
    target_compile_options(player PRIVATE ${CURSES_CFLAGS})
    target_compile_definitions(player PRIVATE USE_CURSES=1)
    target_include_directories(player PRIVATE ${CURSES_INCLUDE_DIRS})
    target_link_libraries(player ${CURSES_LIBRARIES})
endif ()