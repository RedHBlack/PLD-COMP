# Welcome to the Documentation

## Overview

This project implements a C compiler with a focus on generating Intermediate Representation (IR), performing code analysis, and generating assembly code. It includes functionalities such as syntax checking, control flow graph (CFG) generation, and code optimization.

## Features

- **C Syntax Analysis:** Parses C source code and performs syntax checks.
- **Intermediate Representation (IR):** Generates and manipulates IR for code analysis.
- **Assembly Generation:** Converts IR into assembly code for various platforms.
- **CFG Generation:** Generates control flow graphs for visualizing program execution.
- **Code Checking:** Validates the code for errors and potential optimizations.

## How to Use

To compile and run the compiler:

1. Clone or download the repository.
2. Set up the build environment.
3. Compile the source code using `make` or the appropriate build command.
4. Run the compiler with the C source file as an argument:  
   `./ifcc path/to/file.c`

## Dependencies

- **ANTLR:** For parsing C source code.
- **Graphviz:** For generating CFG visualizations.

## Project Structure

- **src/:** Source code for the compiler.
- **include/:** Header files defining the compiler's functionality.
- **test/:** Test files for validating the compiler's correctness.
- **docs/:** Documentation for the project.
- **README.md:** Project overview and setup instructions.

## License

This project is licensed under the BJAQPIG License.
