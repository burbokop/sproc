cmake_minimum_required(VERSION 2.8)

set(SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/pipe_container.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/fork.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/apt.cpp
)

set(HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/src/pipe_container.h
    ${CMAKE_CURRENT_LIST_DIR}/src/fork.h
    ${CMAKE_CURRENT_LIST_DIR}/src/apt.h
    )

add_library(sproc SHARED
    ${SOURCES}
    ${HEADERS}
)
