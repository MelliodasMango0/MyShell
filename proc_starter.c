#include "mio.h"
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    
    // Initialize custom I/O streams
    minit();

    // Create Pipe 1 and Pipe 2
    int pipe1[2], pipe2[2];

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        const char *errMessage = "Error creating pipes.\n";
        mwrite(mtderr, errMessage, (int)strlen(errMessage));
        return -1;
    }

    // Create Child 1 (Oper1)
    pid_t child1 = fork();

    if (child1 == -1) {
        const char *errMessage = "Error creating Child 1 (Oper1).\n";
        mwrite(mtderr, errMessage, (int)strlen(errMessage));
        return -1;
    }

    if (child1 == 0) {
        // Child 1 (Oper1) - Word Replacer
        close(pipe1[1]);  // Close write end of Pipe 1
        close(pipe2[0]);  // Close read end of Pipe 2

        // Redirect Pipe 1 to stdin and Pipe 2 to stdout
        dup2(pipe1[0], STDIN_FILENO);
        dup2(pipe2[1], STDOUT_FILENO);

        // Execute WordReplacer with the provided filename as an argument
        execlp("word_replacer", "child_1","rwords.txt", (char *)NULL);

        // If execlp fails, report an error
        const char *errMessage = "Error executing WordReplacer.\n";
        mwrite(mtderr, errMessage, (int)strlen(errMessage));
        return -1;
    } else {
        pid_t child2 = fork();
        
        if (child2 == -1) {
            const char *errMessage = "Error creating Child 2 (Oper2).\n";
            mwrite(mtderr, errMessage, (int)strlen(errMessage));
            return -1;
        }

        if (child2 == 0) {
            // Child 2 (Oper2) - Word Counter
            close(pipe1[0]);
            close(pipe1[1]);
            close(pipe2[1]);  // Close write end of Pipe 2

            // Redirect Pipe 2 to stdin
            dup2(pipe2[0], STDIN_FILENO);

            // Execute WordCounter
            execlp("word_counter", "child_2", (char *)NULL);

            // If execlp fails, report an error
            const char *errMessage = "Error executing WordCounter.\n";
            mwrite(mtderr, errMessage, (int)strlen(errMessage));
            return -1;
        } else {
            // Parent process
            close(pipe2[0]);  // Close read end of Pipe 2
            close(pipe2[1]);
            close(pipe1[0]);
            // Create MILE objects for pipe1
            MILE *pipe1_stream = mdopen(pipe1[1], O_WRONLY, 0); // Pass buffer size

            if (pipe1_stream == NULL) {
                const char *errMessage = "Error creating MILE for Pipe 1.\n";
                mwrite(mtderr, errMessage, (int)strlen(errMessage));
                return -1;
            }

            char buffer[0];
            int bytesRead;

             // Loop to get user input
            while (1) {
                int length;
                char* string;
                string = mgets(mtdin, &length);

                if (string == NULL) {
                    break; // Exit the loop on EOF or error
                }

                // Write the acquired string to Pipe 1 using MILE
                if (mputs(pipe1_stream, string, length) == -1) {
                    const char *errMessage = "Error writing to Pipe 1.\n";
                    mputs(mtderr, errMessage, (int)strlen(errMessage));
                    mputc(mtdout, ' ');
                    return -1;
                }
                mputc(pipe1_stream, ' ');
            }
            // Close MILE and write end of Pipe 1 (Child 1 will detect EOF)
            mclose(pipe1_stream);
            //close(pipe1[1]);

            // Wait for Child 1 (Oper1) and Child 2 (Oper2) to complete
            waitpid(child1, NULL, 0);
            waitpid(child2, NULL, 0);
        }
    }

    return 0;
}
