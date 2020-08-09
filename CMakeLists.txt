cmake_minimum_required(VERSION 3.18.0)

project(BLTickRate)

## Add DLL source.

add_subdirectory(Source)

## Force static linking.

if(MSVC)
    set_property(TARGET ${PROJECT_NAME} PROPERTY 
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()