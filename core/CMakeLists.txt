cmake_minimum_required(VERSION 3.16)
project(CosmicCore LANGUAGES CXX)

file(GLOB_RECURSE CB_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(CosmicCore STATIC ${CB_SOURCES}
        src/event/EventHandler.cpp)
add_library(Cosmic::Core ALIAS CosmicCore)

target_include_directories(CosmicCore PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(CosmicCore PUBLIC cxx_std_20)
