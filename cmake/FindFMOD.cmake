set(FMOD_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/lib/fmod-2.00/include)

if(WIN32)
    set(FMOD_LIBRARY_DEBUG ${CMAKE_SOURCE_DIR}/lib/fmod-2.00/lib/x64/windows)

    set(FMOD_LIBRARY_LIB_NAMES fmodL_vc)
    set(FMOD_LIBRARY_LIB_DLL_NAMES fmodL)

    find_library(FMOD_LIBRARY_LIB
        NAMES ${FMOD_LIBRARY_LIB_NAMES}
        PATHS ${FMOD_LIBRARY_DEBUG}
        PATH_SUFFIXES lib
    )

    find_file(FMOD_LIBRARY_LIB_DLL fmodL.dll
        NAMES ${FMOD_LIBRARY_LIB_DLL_NAMES}
        PATHS ${FMOD_LIBRARY_DEBUG}
        PATH_SUFFIXES dll
    )

    message(FMOD_LIBRARY_LIB:${FMOD_LIBRARY_LIB})
    message(FMOD_LIBRARY_LIB_DLL:${FMOD_LIBRARY_LIB_DLL})

else()
    set(FMOD_LIBRARY_DEBUG ${CMAKE_SOURCE_DIR}/lib/fmod-2.00/lib/x64/linux)
    set(FMOD_LIBRARY_LIB_NAMES fmodL)

    find_library(FMOD_LIBRARY_LIB
        NAMES ${FMOD_LIBRARY_LIB_NAMES}
        PATHS ${FMOD_LIBRARY_DEBUG}
        PATH_SUFFIXES so
    )

    message(FMOD_LIBRARY_LIB:${FMOD_LIBRARY_LIB})

endif()
