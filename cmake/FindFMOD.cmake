# set(FMOD_LIBRARY ${CMAKE_SOURCE_DIR}/lib/fmod-2.00/lib/x64/fmodL_vc.lib) #Windows
set(FMOD_LIBRARY_DEBUG ${CMAKE_SOURCE_DIR}/lib/fmod-2.00/lib/x64) #Windows
set(FMOD_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/lib/fmod-2.00/include)

set(FMOD_LIBRARY_LIB_NAMES fmodL_vc) #Windows
set(FMOD_LIBRARY_LIB_DLL_NAMES fmodL) #Windows

# Windows
find_library(FMOD_LIBRARY_LIB
    NAMES ${FMOD_LIBRARY_LIB_NAMES}
    PATHS ${FMOD_LIBRARY_DEBUG}
    PATH_SUFFIXES lib
)

# Windows
find_file(FMOD_LIBRARY_LIB_DLL fmodL.dll
    NAMES ${FMOD_LIBRARY_LIB_DLL_NAMES}
    PATHS ${FMOD_LIBRARY_DEBUG}
    PATH_SUFFIXES dll
)

message(${FMOD_LIBRARY_LIB})
message(${FMOD_LIBRARY_LIB_DLL}) # Windows
