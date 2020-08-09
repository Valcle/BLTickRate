## Create the DLL.

# Define private directory (no public directory, this is a hack).

set(BLTR_PRIVATE_DIR ${PROJECT_SOURCE_DIR}/Source)

# Include sources.

set(BLTR_MAIN_SOURCES
    ${BLTR_PRIVATE_DIR}/Main.cpp
)

source_group(Source FILES ${BLTR_MAIN_SOURCES})

## Add files to library.

add_library(${PROJECT_NAME}
    SHARED
        ${BLTR_MAIN_SOURCES}
)

## Create an alias.

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

## Specify private include directories.

target_include_directories(${PROJECT_NAME} PRIVATE ${BLTR_PRIVATE_DIR})