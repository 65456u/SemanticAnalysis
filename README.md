
# Semantic Analysis

## Description

The SemanticAnalysis project provides a C++ implementation for parsing and analyzing context-free grammars using LR(1) parsing technique.

## Usage Instructions

To use the SemanticAnalysis project, follow these steps:

1. **Building the Project**:
```shell
# Create a build directory:
mkdir build
# Generate build files using CMake
cmake ..
# Build the project
make
```

2. **Running the Program**:
```shell
# Execute the compiled executable to observe LL(1) or LR(1) parsing in Action:
./SemanticAnalysis [LL]
```

   The program will prompt you to input a string of tokens. It will then parse the input using the LL(1) parsing table generated from the sample grammar provided in the `main.cpp` file.

## Built-in Grammar

The built-in grammar is as follows:

```
E -> E+T | E-T | T
T -> T*F | T/F | F
F -> (E) | num
```

You can alter the grammar in the `main.cpp` file.

## Sample Usage

1. **Input**:
   
   ```
   num
   ```
   
2. **Output**:
   ```
   STACK   SYMBOLS INPUT   ACTION
   0               num$    SHIFT
   05      num     $       REDUCE F->num
   03      F       $       REDUCE T->F
   04      T       $       REDUCE E->T
   02      E       $       ACCEPT
   ```
