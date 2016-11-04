cmake_minimum_required(VERSION 3.6)
project(Projeto_de_compiladores)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    compilador.c)

add_executable(Projeto_de_compiladores ${SOURCE_FILES})