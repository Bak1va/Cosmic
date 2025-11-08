cmake_minimum_required(VERSION 3.16)
project(CosmicGUI LANGUAGES CXX)

file(GLOB_RECURSE CB_SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(CosmicGUI STATIC ${CB_SOURCES})
add_library(Cosmic::GUI ALIAS CosmicGUI)

target_include_directories(CosmicGUI PRIVATE ${CMAKE_SOURCE_DIR}/CosmicBusiness/src)

target_link_libraries(CosmicGUI PRIVATE Cosmic::Core)

target_compile_features(CosmicGUI PUBLIC cxx_std_20)
