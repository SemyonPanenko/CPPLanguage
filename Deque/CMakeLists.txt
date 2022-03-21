cmake_minimum_required(VERSION 3.16)

project(Deque)

add_library(DequeLib deque.cpp)
add_executable(DequePlay deque_play.cpp)
target_link_libraries(DequePlay DequeLib)