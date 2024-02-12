#include "mio.h"
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

void init_shell();
void print_prompt();
void print_help();
void print_exit_message();
void parse_and_execute_command(char *input_command);
void handle_run_command(char *input_command);
void execute_program(char *program, char **arguments);
void execute_program_with_pipe(char *program1, char **arguments1, char *program2, char **arguments2);
void execute_program_with_redirection(char *program, char **arguments, char *filename);
void execute_program_with_input_redirection(char *program, char **arguments, char *filename);
void execute_program_with_tcp_redirection(char *program, char **arguments, char *hostname, char *port);
void parse_arguments(char *input_command, char **program, char ***arguments, int *arg_count);
void free_arguments(char **arguments, int arg_count);

void init_shell() {
    char *welcome_message = "Welcome to MyShellv2 by Jose Cardenas\n";
    mputs(mtdout, welcome_message, strlen(welcome_message));
    char *license = "Copyright: Jose Cardenas 2023\n";
    mputs(mtdout, license, strlen(license));
    mputc(mtdout, '\n');
}

void print_prompt() {
    const char *shell = "Myshell>";
    mputs(mtdout, shell, strlen(shell));
}

void print_help() {
    char *help_text = "Commands: 'Help', 'Quit', 'Run <program> [<arg1> <arg2> …]' or 'Run <program1> [<arg1.1> <arg1.2> …] Pipe <program2> [<arg2.1> <arg2.2> …]'\n";
    mputs(mtdout, help_text, strlen(help_text));
}

void print_exit_message() {
    char *exit_text = "Thank you for using MyShell by Jose Cardenas\nContributions to support development can be sent to:\nECE Department, College of Engineering, University of Miami\nc/o Dr. Nigel John\n";
    mputs(mtdout, exit_text, strlen(exit_text));
}

void parse_and_execute_command(char *input_command) {
    if (strcmp(input_command, "Help") == 0) {
        print_help();
    } else if (strcmp(input_command, "Quit") == 0) {
        print_exit_message();
        exit(0);
    } else if (strncmp(input_command, "Run", 3) == 0) {
        handle_run_command(input_command);
    } else {
        char *unknown = "Unknown Command, Use 'Help' for a list of commands\n";
        mputs(mtdout, unknown, strlen(unknown));
    }
}

void handle_run_command(char *input_command) {
    char *token;
    char *program1 = NULL;
    char *program2 = NULL;
    char *filename = NULL;
    char **arguments1 = NULL;
    char **arguments2 = NULL;
    int arg_count1 = 0;
    int arg_count2 = 0;
    int is_pipe = 0;
    int is_redirection = 0;
    int is_input_redirection = 0;
    int is_tcp_redirection = 0;

    // Tokenize the command
    token = strtok(input_command, " "); // Skips "Run"

    // Get the first program name
    token = strtok(NULL, " ");
    if (token != NULL) {
        program1 = strdup(token);
        arg_count1++;
        arguments1 = (char **)malloc(sizeof(char *));
        arguments1[0] = program1;
    }

    // Parse the rest of the command
    while ((token = strtok(NULL, " ")) != NULL) {
        if (strcmp(token, "Pipe") == 0) {
            // Handle Pipe case
            is_pipe = 1;
            token = strtok(NULL, " ");
            if (token != NULL) {
                program2 = strdup(token);
                arg_count2++;
                arguments2 = (char **)malloc(sizeof(char *));
                arguments2[0] = program2;
            }
            continue;
        } else if (strcmp(token, "To") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                if (strncmp(token, "/TCP/", 5) == 0) {
                    // Handle TCP Redirection case
                    is_tcp_redirection = 1;

                    // Ensure arguments array is null-terminated
                    arguments1 = (char **)realloc(arguments1, sizeof(char *) * (arg_count1 + 1));
                    arguments1[arg_count1] = NULL;

                    char *tcp_info = strdup(token + 5); // Skip "/TCP/"
                    char *hostname = strtok(tcp_info, "/");
                    char *port = strtok(NULL, "/");
                    execute_program_with_tcp_redirection(program1, arguments1, hostname, port);
                    free(tcp_info);
                    break;
                } else {
                    // Handle File Redirection case
                    is_redirection = 1;
                    filename = strdup(token);

                    // Ensure arguments array is null-terminated
                    arguments1 = (char **)realloc(arguments1, sizeof(char *) * (arg_count1 + 1));
                    arguments1[arg_count1] = NULL;

                    break;
                }
            }
        }
        else if (strcmp(token, "From") == 0) {
            // Handle Input Redirection case
            is_input_redirection = 1;
            token = strtok(NULL, " ");
            if (token != NULL) {
                filename = strdup(token);
            }
            break;
        }
        
        // Add to arguments
        if (!is_pipe) {
            arg_count1++;
            arguments1 = (char **)realloc(arguments1, sizeof(char *) * arg_count1);
            arguments1[arg_count1 - 1] = strdup(token);
        } else {
            arg_count2++;
            arguments2 = (char **)realloc(arguments2, sizeof(char *) * arg_count2);
            arguments2[arg_count2 - 1] = strdup(token);
        }
    }

    // Null-terminate argument arrays
    if (arg_count1 > 0) {
        arguments1 = (char **)realloc(arguments1, sizeof(char *) * (arg_count1 + 1));
        arguments1[arg_count1] = NULL;
    }
    if (arg_count2 > 0) {
        arguments2 = (char **)realloc(arguments2, sizeof(char *) * (arg_count2 + 1));
        arguments2[arg_count2] = NULL;
    }

    // Execute based on command type
    if (is_pipe) {
        execute_program_with_pipe(program1, arguments1, program2, arguments2);
    } else if (is_redirection) {
        execute_program_with_redirection(program1, arguments1, filename);
    } else if (is_input_redirection){
        execute_program_with_input_redirection(program1, arguments1, filename);
    } else {
        execute_program(program1, arguments1);
    }

    // Free memory
    for (int i = 0; i < arg_count1; i++) {
        free(arguments1[i]);
    }
    free(arguments1);
    if (is_pipe) {
        for (int i = 0; i < arg_count2; i++) {
            free(arguments2[i]);
        }
        free(arguments2);
    }
    if (is_redirection) {
        free(filename);
    }
}


void execute_program(char *program, char **arguments) {
    pid_t pid = fork(); // Create a new process

    if (pid == -1) {
        // Fork failed
        mputs(mtdout, "Error: Unable to fork process\n", 30);
    } else if (pid == 0) {
        // Child process
        execvp(program, arguments); // Replace the child process with the new program

        // If execvp returns, it must have failed
        mputs(mtdout, "Error: Execution failed\n", 24);
        exit(1); // Exit with an error status
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0); // Wait for the child process to finish

        mputs(mtdout, "Child process has finished.\n", 29);
    }
}

void execute_program_with_pipe(char *program1, char **arguments1, char *program2, char **arguments2) {
    int pipe_fd[2]; // File descriptors for the pipe

    // Create a pipe
    if (pipe(pipe_fd) < 0) {
        mputs(mtderr, "Error creating pipe\n", 20);
        return;
    }

    // Fork the first child process
    pid_t child_pid1 = fork();
    
    if (child_pid1 < 0) {
        mputs(mtderr, "Error forking first child process\n", 34);
        return;
    } else if (child_pid1 == 0) {
        // First child process
        close(pipe_fd[0]); // Close read end of the pipe
        dup2(pipe_fd[1], STDOUT_FILENO); // Redirect STDOUT to the pipe
        close(pipe_fd[1]); // Close the original file descriptor

        // Execute the first program
        if (execvp(program1, arguments1) < 0) {
            mputs(mtderr, "Error executing first program\n", 30);
            exit(1);
        }
    }

    // Fork the second child process
    pid_t child_pid2 = fork();

    if (child_pid2 < 0) {
        mputs(mtderr, "Error forking second child process\n", 35);
        return;
    } else if (child_pid2 == 0) {
        // Second child process
        close(pipe_fd[1]); // Close write end of the pipe
        dup2(pipe_fd[0], STDIN_FILENO); // Redirect STDIN from the pipe
        close(pipe_fd[0]); // Close the original file descriptor

        // Execute the second program
        if (execvp(program2, arguments2) < 0) {
            mputs(mtderr, "Error executing second program\n", 31);
            exit(1);
        }
    }

    // Parent process
    close(pipe_fd[0]); // Close read end
    close(pipe_fd[1]); // Close write end

    // Wait for both child processes to complete
    int status1, status2;
    waitpid(child_pid1, &status1, 0);
    waitpid(child_pid2, &status2, 0);
    
}

void execute_program_with_redirection(char *program, char **arguments, char *filename) {
    pid_t pid = fork(); // Create a new process

    if (pid == -1) {
        // Fork failed
        mputs(mtderr, "Error: Unable to fork process\n", 30);
    } else if (pid == 0) {
        // Child process
        // Open the file for writing
        int file_fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (file_fd < 0) {
            mputs(mtderr, "Error: Unable to open file for redirection\n", 44);
            exit(1); // Exit with an error status
        }

        // Redirect STDOUT to the file
        dup2(file_fd, STDOUT_FILENO);
        close(file_fd); // Close the file descriptor, as it's no longer needed

        // Execute the program
        execvp(program, arguments);

        // If execvp returns, it must have failed
        mputs(mtderr, "Error: Execution failed\n", 24);
        exit(1); // Exit with an error status
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0); // Wait for the child process to finish


    }
}

void execute_program_with_input_redirection(char *program, char **arguments, char *filename) {
    // Open the file for reading
    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0) {
        mputs(mtderr, "Error: Unable to open file for input redirection\n", 50);
        return;
    }

    pid_t pid = fork(); // Create a new process

    if (pid == -1) {
        // Fork failed
        mputs(mtderr, "Error: Unable to fork process\n", 30);
    } else if (pid == 0) {
        // Child process
        // Redirect STDIN to the file
        dup2(file_fd, STDIN_FILENO);
        close(file_fd); // Close the file descriptor, as it's no longer needed

        // Execute the program
        execvp(program, arguments);

        // If execvp returns, it must have failed
        mputs(mtderr, "Error: Execution failed\n", 24);
        exit(1); // Exit with an error status
    } else {
        // Parent process
        close(file_fd); // Close the file descriptor
        int status;
        waitpid(pid, &status, 0); // Wait for the child process to finish

        // Optionally, handle the exit status of the child process here
    }
}

void execute_program_with_tcp_redirection(char *program, char **arguments, char *hostname, char *port) {
    
    struct addrinfo hints, *res0;
    int commsoc;
    int error;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    error = getaddrinfo(hostname, port, &hints, &res0);
    if (error) {
        const char *errMsg = gai_strerror(error);
        mputs(mtderr, errMsg, strlen(errMsg));
        mputs(mtderr, "\n", 1);
        return;
    }

    commsoc = socket(res0->ai_family, res0->ai_socktype, res0->ai_protocol);
    if (commsoc < 0) {
        mputs(mtderr, "cannot get socket\n", 19);
        freeaddrinfo(res0);
        return;
    }

    if (connect(commsoc, res0->ai_addr, res0->ai_addrlen) < 0) {
        mputs(mtderr, "cannot connect\n", 16);
        close(commsoc);
        freeaddrinfo(res0);
        return;
    }

    pid_t pid = fork();
    if (pid == -1) {
        mputs(mtderr, "Error: Unable to fork process\n", 30);
        close(commsoc);
        freeaddrinfo(res0);
        return;
    } else if (pid == 0) {
        dup2(commsoc, STDOUT_FILENO);
        close(commsoc);
        freeaddrinfo(res0);
        execvp(program, arguments);
        mputs(mtderr, "Error: Execution failed\n", 24);
        exit(1);
    } else {
        close(commsoc);
        freeaddrinfo(res0);
        int status;
        waitpid(pid, &status, 0);
    }
}

void parse_arguments(char *input_command, char **program, char ***arguments, int *arg_count) {
    *arg_count = 0; // Initialize the argument count to 0
    *arguments = NULL; // Initialize the arguments array to NULL

    // Tokenize the input command using space as a delimiter
    char *token = strtok(input_command, " ");

    // The first token is the program name
    *program = strdup(token);

    // Process the rest of the tokens as arguments
    while ((token = strtok(NULL, " ")) != NULL) {
        // Increase arg_count and reallocate memory for arguments array
        (*arg_count)++;
        *arguments = realloc(*arguments, sizeof(char *) * (*arg_count));

        // Duplicate the token and store it in the arguments array
        (*arguments)[*arg_count - 1] = strdup(token);
    }

    // Null-terminate the arguments array
    *arguments = realloc(*arguments, sizeof(char *) * (*arg_count + 1));
    (*arguments)[*arg_count] = NULL;
}

void free_arguments(char **arguments, int arg_count) {
    // Free each argument string
    for (int i = 0; i < arg_count; i++) {
        free(arguments[i]);
    }

    // Free the array of pointers itself
    free(arguments);
}

int main() {
    
    minit();
    init_shell();
    print_prompt();

    char *input_command;
    int length;

    while ((input_command = mgetline(mtdin, &length))) {
        parse_and_execute_command(input_command);
        print_prompt();
    }

    return 0;
}
