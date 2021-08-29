add_executable(example src/Sandbox.cpp src/utils/FileLoaderHelper.c)

target_link_libraries(example univer_audio)
message(${CMAKE_SOURCE_DIR}/include)
target_include_directories(example PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Windows
message(${CMAKE_BUILD_TYPE})
file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
file(COPY ${FMOD_LIBRARY_LIB_DLL} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
