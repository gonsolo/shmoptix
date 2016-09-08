cmake_minimum_required(VERSION 3.5)
project(Show)
add_executable(show show.cc)
add_custom_target(t COMMAND ./show)
add_custom_target(e COMMAND vi ../show.cc)

