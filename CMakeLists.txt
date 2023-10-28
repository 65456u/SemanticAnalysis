cmake_minimum_required(VERSION 3.22)
project(SemanticAnalysis)

set(CMAKE_CXX_STANDARD 17)

add_executable(SemanticAnalysis main.cpp
        Grammar.cpp
        Grammar.h
        utils.h
        LL1Grammar.cpp
        LL1Grammar.h)
