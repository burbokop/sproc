cmake_minimum_required(VERSION 2.8)

set(SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/sproc.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/src/sproc.h
    )

add_library(sproc SHARED
    ${SOURCES}
    ${HEADERS}
)
