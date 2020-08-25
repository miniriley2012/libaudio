add_library("${PROJECT_NAME}" libaudio.cpp apple/player_impl.mm)

if (APPLE)
    target_sources("${PROJECT_NAME}" PRIVATE apple/player_impl.mm)
    target_compile_options("${PROJECT_NAME}" PRIVATE "-fobjc-arc")
else ()
    message(WARNING "Missing player implementation for your system. This most likely isn't your fault.")
endif ()

target_include_directories("${PROJECT_NAME}" PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_link_libraries("${PROJECT_NAME}" PUBLIC "-framework CoreFoundation -framework AVFoundation")