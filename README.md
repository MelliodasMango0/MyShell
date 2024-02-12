# MyShell
Enhance your command-line efficiency with a versatile suite of utilities for process management and text processing, including a custom shell, process starter, word counter, and word replacer.

## Project Overview

This suite of command-line utilities is designed to offer powerful, yet easy-to-use tools that extend your shell's capabilities, simplify process management, and provide robust text manipulation functionalities. Whether you're automating routine tasks, managing processes, or manipulating text data, this toolkit is built to improve your productivity and make command-line operations more efficient and intuitive.

### Features

- **Custom Shell (shell2.c)**: An interactive command-line interface that allows for executing commands, managing background processes, and enhancing your command-line navigation and operation capabilities.
- **Process Starter (proc_starter.c)**: Simplifies the initiation and management of processes, offering a straightforward way to run commands or scripts in the background or foreground, with enhanced control over their execution.
- **Word Counter (word_counter.c)**: Quickly counts words in a given text file or input stream, providing valuable insights for text analysis, content creation, and data processing tasks.
- **Word Replacer (word_replacer.c)**: A handy tool for replacing words or phrases in text files, perfect for batch editing, content updates, and automating the correction of common errors or updates in documentation or code comments.

## Getting Started

### Prerequisites

Before you begin, ensure you have the following installed on your system:
- A C compiler (GCC recommended) to compile the utilities.
- Basic knowledge of command-line operations and shell environments.

### Installation

To set up the Command-Line Utilities Suite on your system, follow these steps:

1. Clone the repository to your local machine:
```bash
git clone <repository-url>
```

2. Navigate to the cloned directory and compile each utility using your C compiler. For example, with GCC:
```bash
gcc -o shell shell2.c
gcc -o proc_starter proc_starter.c
gcc -o word_counter word_counter.c
gcc -o word_replacer word_replacer.c
```

3. You can now run any of the compiled utilities directly from your command line by specifying the utility's name followed by its required arguments.

### Usage Examples

- **Launching the Custom Shell**:
```bash
./shell
```
Once launched, you can execute commands directly within the custom shell environment.

- **Starting a Process**:
```bash
./proc_starter 'command_or_script'
```
Replace `'command_or_script'` with the command or script you wish to run.

- **Counting Words in a Text File**:

./word_counter input.txt

Replace `input.txt` with the path to your text file.

- **Replacing Words in a Text File**:

./word_replacer input.txt 'target_word' 'replacement_word'

Specify the file, the word to replace, and the new word.


