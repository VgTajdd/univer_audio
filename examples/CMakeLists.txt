add_executable(example src/Sandbox.cpp src/utils/FileLoaderHelper.c)

target_link_libraries(example univer_audio)
target_include_directories(example PUBLIC ${CMAKE_SOURCE_DIR}/include)

message(CMAKE_CURRENT_BINARY_DIR:${CMAKE_CURRENT_BINARY_DIR})
message(CMAKE_BUILD_TYPE:${CMAKE_BUILD_TYPE})

if(WIN32)
    file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
    file(COPY ${FMOD_LIBRARY_LIB_DLL} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
else()
    file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()
