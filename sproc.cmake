cmake_minimum_required(VERSION 3.18.4)

set(SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/pipe_container.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/fork.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/apt.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/environment.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/cache.cpp
)

set(HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/src/pipe_container.h
    ${CMAKE_CURRENT_LIST_DIR}/src/fork.h
    ${CMAKE_CURRENT_LIST_DIR}/src/apt.h
    ${CMAKE_CURRENT_LIST_DIR}/src/environment.h
    ${CMAKE_CURRENT_LIST_DIR}/src/cache.h
    )

add_library(sproc SHARED
    ${SOURCES}
    ${HEADERS}
)
set_property(TARGET sproc PROPERTY AUTOMOC OFF)
set_property(TARGET sproc PROPERTY AUTOUIC OFF)


install(TARGETS sproc DESTINATION lib)

install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/src/"
        DESTINATION include/sproc
        FILES_MATCHING
        PATTERN "*.h"
)
