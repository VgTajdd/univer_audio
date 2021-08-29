cmake_minimum_required(VERSION 3.20.0)
project(univer_audio VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

add_library(univer_audio STATIC
    src/UAudioEngine.cpp
    src/UAudioEngine.cpp
    src/UAudioFader.cpp
    src/USound.cpp
)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(FMOD MODULE REQUIRED)

target_include_directories(univer_audio PUBLIC ${FMOD_INCLUDE_DIR} include)

target_link_libraries(univer_audio ${FMOD_LIBRARY_LIB}) #Windows

if(NOT UNIVER_AUDIO_ONLY_LIB)
    add_subdirectory(examples)
endif()

# References.
# https://stackoverflow.com/questions/48187111/link-external-library-cmakelists-txt-on-windows
# https://github.com/hlrs-vis/covise/blob/master/cmake/FindFMOD.cmake
