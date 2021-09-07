cmake_minimum_required(VERSION 3.16.0)
project(univer_audio VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

file(GLOB UNIVER_AUDIO_SOURCES "src/*.cpp")
add_library(univer_audio STATIC ${UNIVER_AUDIO_SOURCES})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(FMOD MODULE REQUIRED)
target_link_libraries(univer_audio ${FMOD_LIBRARY_LIB})
target_include_directories(univer_audio PUBLIC ${FMOD_INCLUDE_DIR} include)

if(NOT UNIVER_AUDIO_ONLY_LIB)
    add_subdirectory(examples)
endif()

# References.
# https://stackoverflow.com/questions/48187111/link-external-library-cmakelists-txt-on-windows
# https://github.com/hlrs-vis/covise/blob/master/cmake/FindFMOD.cmake
