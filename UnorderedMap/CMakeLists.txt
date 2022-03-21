cmake_minimum_required(VERSION 3.16)

project(UnorderedMap)

add_library(UnorderedMapLib unordered_map.cpp)
add_executable(UnorderedMapPlay unordered_map_play.cpp)
target_link_libraries(UnorderedMapPlay UnorderedMapLib)