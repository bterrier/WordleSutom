cmake_minimum_required(VERSION 3.10)

project(WordleSutom LANGUAGES CXX)


add_library(LibWordleSutom
    Application.cpp Application.h
    gamestate.h
)

target_include_directories(
    LibWordleSutom
    PUBLIC
    .
    )

add_executable(WordleSutom
    main.cpp
)

target_link_libraries(WordleSutom
    PRIVATE
        LibWordleSutom)

add_subdirectory(tests)
