cmake_minimum_required(VERSION 3.15)
project(Zone)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# VST3 SDK
add_subdirectory(vst3/sdk)

# Source files
add_library(zone MODULE
    src/Processor.cpp
    src/Controller.cpp
    src/Entry.cpp
)

target_include_directories(zone PRIVATE
    src
    vst3/sdk
)

# VST3-specific properties
set_target_properties(zone PROPERTIES
    FRAMEWORK TRUE
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_NAME "Zone"
    MACOSX_BUNDLE_EXTENSION "vst3"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.zone.vst3"
    MACOSX_BUNDLE_ICON_FILE "Icon.icns"
    MACOSX_BUNDLE_INFO_PLIST "Info.plist"
)

# Create the VST3 bundle directory structure
add_custom_target(ZoneVST3 ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory
            "${CMAKE_BINARY_DIR}/Zone.vst3/Contents/MacOS"
    COMMAND ${CMAKE_COMMAND} -E copy
            "$<TARGET_FILE:zone>"
            "${CMAKE_BINARY_DIR}/Zone.vst3/Contents/MacOS/"
    DEPENDS zone
)
