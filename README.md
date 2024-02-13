# MyShell

## Overview
This project develops a custom shell environment, enhancing command-line interaction by supporting process management, I/O redirection, and network communication. It utilizes C language for creating a flexible and efficient shell capable of executing commands, managing processes, and handling file and network I/O operations.

## Components

### mio.c & mio.h
Implements a minimalistic I/O library, providing essential functions for input/output operations within the shell, facilitating streamlined data handling.

### proc_starter.c
Focuses on initializing and managing processes. It includes functions to start, stop, and monitor the status of processes, integrating closely with the shell's command execution framework.

### shell2.c
Acts as the core of the custom shell, implementing the user interface, command parsing, and execution logic. It supports executing simple commands, as well as advanced features like piping, redirection, and TCP redirection for network communication.

### word_replacer.c
Provides functionality to replace specified words in the input stream. This can be used for filtering output or modifying commands before execution.

### word_counter.c
Counts occurrences of words, useful for analyzing command output or input stream content, offering insights into data processed by the shell.

## Installation

To compile the project, ensure GCC or an equivalent compiler supporting C is installed. Use the makefile provided or compile manually with `gcc -o custom_shell shell2.c mio.c proc_starter.c word_replacer.c word_counter.c -I.`

## Usage

After compilation, run `./custom_shell` to start the shell. Use standard shell commands, along with the custom functionalities provided by the project. For specific features like word replacement or counting, refer to the internal documentation or help command integrated within the shell.

##Files to test processes

alice2.txt
rwords.txt 

PS: Any file works however, you can try with the bible and if you don't run out of memory, it should properly count the words and replace the requested words.


