cmake_minimum_required(VERSION 3.22)
project(SemanticAnalysis)

set(CMAKE_CXX_STANDARD 17)

# Update the source files
set(SOURCE_FILES
        src/main.cpp
        src/Grammar.cpp
        src/LL1Grammar.cpp
        src/LR1Grammar.cpp
        src/LR1Grammar.h
)

# Update the header files
set(HEADER_FILES
        include/utils.h
        src/Grammar.h
        src/LL1Grammar.h
        src/LR1Grammar.cpp
        src/LR1Grammar.h
)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
# Add your executable target
add_executable(SemanticAnalysis ${SOURCE_FILES} ${HEADER_FILES})