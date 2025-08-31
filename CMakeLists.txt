cmake_minimum_required(VERSION 3.29)
project(NoTrackingMarkers C CXX)

set(CMAKE_CXX_STANDARD 20)

enable_language(RC)

find_package(Boost REQUIRED COMPONENTS url json)

add_executable(NoTrackingMarkers WIN32 main.cpp ntm.rc NoTrackingMarkers.exe.manifest)
target_link_libraries(NoTrackingMarkers PRIVATE Boost::url Boost::json ComCtl32.lib shlwapi.lib)