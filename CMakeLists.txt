cmake_minimum_required(VERSION 3.22)
project(SemanticAnalysis)

set(CMAKE_CXX_STANDARD 17)

add_executable(SemanticAnalysis main.cpp
        src/Grammar.cpp
        src/Grammar.h
        include/utils.h
        src/LL1Grammar.cpp
        src/LL1Grammar.h)
