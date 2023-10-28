# Semantic Analysis

## Description

The SemanticAnalysis project provides a C++ implementation for parsing and analyzing context-free grammars using LL(1) parsing technique.

## Usage Instructions

To use the SemanticAnalysis project, follow these steps:

1. **Building the Project**:
   - Create a build directory: `mkdir build`
   - Generate build files using CMake: `cmake ..`
   - Build the project: `make`

2. **Running the Program**:
   - Execute the compiled executable to observe LL(1) parsing in action:
     ```
     ./SemanticAnalysis
     ```

   The program will prompt you to input a string of tokens. It will then parse the input using the LL(1) parsing table generated from the sample grammar provided in the `main.cpp` file.

## Built-in Grammar

The built-in grammar is as following:
$$
\begin{equation*}
\begin{aligned}
\mathrm{E} & \rightarrow \mathrm{E}+\mathrm{T} \,|\, \mathrm{E}-\mathrm{T} \,|\, \mathrm{T} \\
\mathrm{T} & \rightarrow \mathrm{T}*\mathrm{F} \,|\, \mathrm{T}/\mathrm{F} \,|\, \mathrm{F} \\
\mathrm{F} & \rightarrow (\mathrm{E}) \,|\, \mathrm{num}
\end{aligned}
\end{equation*}
$$
 You can alter the grammar in the `main.cpp` file.

## Sample Usage

1. **Input**:
   
   ```
   num
   ```
   
2. **Output**:
   ```
   Matched Stack   Input   Action
           E$      num$    Output E->TE'
           TE'$    num$    Output T->FT'
           FT'E'$  num$    Output F->num
   num     T'E'$   $       match num
   num     T'E'$   $       Output T'->e
   num     E'$     $       Output E'->e
   num$                    match $
   ```