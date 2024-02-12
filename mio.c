#include "mio.h"

// Global MILE pointers for standard I/O streams
MILE *mtdin = NULL;
MILE *mtdout = NULL;
MILE *mtderr = NULL;

// Function to initialize standard I/O streams
void minit(void) {
    // Open standard input (0) for reading with a buffer size of 1024
    mtdin = mdopen(0, MODE_R, 0);

    // Open standard output (1) for writing with append mode and a buffer size of 1024
    mtdout = mdopen(1, MODE_WA, 0);

    // Open standard error (2) for writing with append mode and a buffer size of 1024
    mtderr = mdopen(2, MODE_WA, 0);
}

// Function to open a file with a given name, mode, and buffer size
MILE *mopen(const char *name, const int mode, const int bsize) {
    int myfd = -1;

    // Determine the file opening mode based on the provided 'mode'
    switch (mode) {
        case MODE_R:
            myfd = open(name, O_RDONLY);
            break;
        case MODE_WA:
            myfd = open(name, O_WRONLY | O_CREAT | O_APPEND, 0644);
            break;
        case MODE_WT:
            myfd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            break;
        default:
            return NULL; // Invalid mode
    }

    if (myfd == -1) {
        return NULL; // Error opening the file
    }

    // Allocate memory for the MILE structure
    MILE *file = (MILE *)malloc(sizeof(MILE));
    if (file == NULL) {
        return NULL; // Memory allocation error
    }

    file->fd = myfd;
    file->rw = mode;

    if (bsize > 0) { // Buffered operation
        file->rb = (char *)malloc(sizeof(char) * MBSIZE);
        file->wb = (char *)malloc(sizeof(char) * MBSIZE);

        if (file->rb == NULL || file->wb == NULL) {
            free(file);
            return NULL; // Memory allocation error for buffers
        }

        file->rsize = bsize;
        file->wsize = bsize;
    } else { // Unbuffered operation
        file->rb = NULL;
        file->wb = NULL;
        file->rsize = 0;
        file->wsize = 0;
    }

    file->rs = 0;
    file->re = 0;
    file->ws = 0;
    file->we = 0;

    return file;
}

// Function to open a file using an existing file descriptor with a given mode and buffer size
MILE *mdopen(const int fd, const int mode, const int bsize) {
    // Allocate memory for the MILE structure
    MILE *file = (MILE *)malloc(sizeof(MILE));
    if (file == NULL) {
        return NULL; // Memory allocation error
    }

    // Connect the provided file descriptor
    file->fd = fd;

    // Store the mode
    file->rw = mode;

    // Store the buffer size
    file->bsize = bsize;

    // Initialize buffers and related fields based on buffer size
    if (bsize > 0) { // Buffered operation
        file->rb = (char *)malloc(sizeof(char) * bsize);
        file->wb = (char *)malloc(sizeof(char) * bsize);

        if (file->rb == NULL || file->wb == NULL) {
            // Memory allocation error for buffers
            free(file->rb);
            free(file->wb);
            free(file);
            return NULL;
        }

        file->rsize = bsize;
        file->wsize = bsize;
        file->rs = 0;
        file->re = 0;
        file->ws = 0;
        file->we = 0;
    } else { // Unbuffered operation
        file->rb = NULL;
        file->wb = NULL;
        file->rsize = 0;
        file->wsize = 0;
        file->rs = 0;
        file->re = 0;
        file->ws = 0;
        file->we = 0;
    }

    return file;
}

// Function to close a file and free associated resources
int mclose(MILE *m) {
    if (!m) return -2; // Invalid MILE pointer

    // Flush the write buffer if it has data
    if (m->we > 0) {
        int bytes_written = write(m->fd, m->wb, m->we);
        if (bytes_written <= 0) {
            return -1; // Error writing remaining data
        }
    }

    // Close the file descriptor
    close(m->fd);

    // Free memory allocated for buffers and MILE structure
    if (m->rb) free(m->rb);
    if (m->wb) free(m->wb);
    free(m);

    return 0; // Successful closure
}

// Function to read data from a file into a buffer
int mread(MILE *m, char *b, const int size) {
    if (!m || !b || size < 0) {
        return -2; // Invalid argument
    }

    // If unbuffered, read directly from the file to 'b'
    if (m->rsize == 0) {
        int bytes_read = read(m->fd, b, size);
        if (bytes_read > 0) {
            return bytes_read;
        } else {
            return -1; // EOF or error
        }
    }

    int total_bytes_read = 0;
    for (int i = 0; i < size; i++) {
        if (m->re == m->rs) { // If the buffer is exhausted
            int bytes_read = read(m->fd, m->rb, m->rsize);

            if (bytes_read == -1) {
                return -3; // Error reading
            }

            if (bytes_read == 0) { // EOF reached
                if (i == 0) return -1;
                else return i; // Return the number of bytes read before EOF
            }

            m->re = bytes_read; // Update end index with the number of bytes read
            m->rs = 0; // Reset the start index
        }

        b[i] = m->rb[m->rs];
        m->rs++;
        total_bytes_read++;
    }

    return total_bytes_read;
}

// Function to read a character from a file
int mgetc(MILE *m, char *c) {
    return mread(m, c, 1);
}

// Function to read a string from a file until a whitespace character is encountered
char *mgets(MILE *m, int *length) {
    char *result_string = NULL;
    char current_char;
    int string_length = 0;

    // Skip leading whitespace characters
    while (1) {
        int error_code = mgetc(m, &current_char);
        if (error_code == -1) {
            if (string_length == 0) {
                *length = 0;
                return NULL;
            } else {
                break;
            }
        }
        if (current_char == ' ' || current_char == '\t' || current_char == '\n' || current_char == '\r') {
            if (string_length == 0) {
                continue;
            } else {
                break;
            }
        }

        result_string = (char *)realloc(result_string, string_length + 1);
        result_string[string_length] = current_char;
        string_length++;
    }

    *length = string_length;

    if (string_length > 0) {
        result_string = (char *)realloc(result_string, string_length + 1);
        result_string[string_length] = '\0';
        return result_string;
    } else {
        return NULL;
    }
}

// Function to read an integer from a file
int mgeti(MILE *m, int *val) {
    int length = 0;
    char *myString = mgets(m, &length); // Get a string from the file
    if (myString == NULL) return -1; // mgets returns NULL on EOF, so return -1 on EOF

    // Check that the string is a valid integer (allowing a negative sign at the start)
    for (int i = 0; i < length; i++) {
        if (!isdigit(myString[i])) {
            if (myString[i] == '-' && i == 0) continue; // Negative sign at the start
            else return -1; // Invalid character in the string
        }
    }

    *val = (int)strtol(myString, NULL, 10);
    return 0;
}

int mwrite(MILE *m, const char *b, const int size) {
    if (m == NULL || m->fd < 0 || (m->rw != MODE_WA && m->rw != MODE_WT)) return -2;

    if (m->wsize <= 0) return (int)write(m->fd, b, size);

    int charsWritten = 0;
    int bufferSpace = m->wsize - m->we; // Calculate remaining buffer space

    while (charsWritten < size) {
        int bytesToWrite = (size - charsWritten < bufferSpace) ? (size - charsWritten) : bufferSpace;

        // Copy bytes from the input buffer to the MILE buffer
        memcpy(&(m->wb[m->we]), &(b[charsWritten]), bytesToWrite);

        charsWritten += bytesToWrite;
        m->we += bytesToWrite;

        if (m->we >= m->wsize) { // If the buffer is full
            int x = mflush(m); // Flush the buffer
            if (x == -1) return -1;
            bufferSpace = m->wsize - m->we; // Update remaining buffer space
        }
    }
    return charsWritten;
}


// Function to flush the write buffer
int mflush(MILE *m) {
    if (!m) return -2; // Invalid MILE pointer
    if (m->we <= 0) return 0; // Nothing to flush

    int bytes_written = write(m->fd, m->wb, m->we); // Attempt to write the buffer to the file

    if (bytes_written < 0) return -1; // Error during write

    m->ws = 0; // Reset the write-start pointer after flushing
    m->we = 0; // Reset the write-end pointer after flushing

    return bytes_written;
}

// Function to write a character to a file
int mputc(MILE *m, const char c) {
    return mwrite(m, &c, 1);
}

// Function to write a string to a file
int mputs(MILE *m, const char *str, const int len) {
    return mwrite(m, str, len);
}

// Function to write an integer to a file
int mputi(MILE *m, const int val) {
    // Figure out how much space to allocate
    int temp = val;
    int length = 0;

    while (temp != 0) {
        length++;
        temp = temp / 10;
    }

    // If 'val' is 0, 'length' will be 0 after the above loop. Write a '0'.
    if (length == 0) {
        return mputc(m, '0');
    }

    if (val < 0) {
        length++; // If 'val' is negative, add an extra byte for the '-' sign.
    }

    // Allocate space for the string to write, plus one for the null-terminator.
    char *myString = (char *)malloc(length + 1);

    if (myString == NULL) {
        return -1; // Memory allocation failed
    }

    temp = val;

    if (val < 0) {
        myString[0] = '-'; // If 'val' is negative, make the first character a negative sign.
        temp = -temp; // Make 'val' positive for int to char conversion.
    }

    // Convert integer to string
    myString[length] = '\0'; // Null-terminate the string
    length--;

    while (temp != 0) {
        int modulo = temp % 10;
        temp = temp / 10;
        char myChar = '0' + modulo;
        myString[length] = myChar;
        length--;
    }

    // Write the string to the MILE structure
    int result = mwrite(m, myString, (int)strlen(myString));

    // Free the allocated memory
    free(myString);

    return result;
}

// Function to read a line from a file
char *mgetline(MILE *m, int *length) {
    char *result_line = NULL;
    char current_char;
    int line_length = 0;

    while (1) {
        int error_code = mgetc(m, &current_char);

        if (error_code == -1) {
            if (line_length == 0) {
                *length = 0;
                return NULL; // No more lines to read
            } else {
                break; // End of file reached after reading a line
            }
        }

        if (current_char == '\n') {
            break; // End of line reached
        }

        result_line = (char *)realloc(result_line, line_length + 1);
        result_line[line_length] = current_char;
        line_length++;
    }

    *length = line_length;

    if (line_length > 0) {
        result_line = (char *)realloc(result_line, line_length + 1);
        result_line[line_length] = '\0'; // Null-terminate the line
        return result_line;
    } else {
        return NULL; // Empty line or end of file
    }
}

//char **mgetline(MILE *m, int *num_tokens) {
//    char **tokens = NULL;
//    char *line = NULL;
//    char current_char;
//    int token_count = 0;
//    int line_length = 0;
//    int token_length = 0;
//    int is_space = 1;
//
//    while (1) {
//        int error_code = mgetc(m, &current_char);
//
//        if (error_code == -1) {
//            if (token_count == 0) {
//                *num_tokens = 0;
//                return NULL; // No more lines to read
//            } else {
//                break; // End of file reached after reading tokens
//            }
//        }
//
//        if (current_char == '\n') {
//            break; // End of line reached
//        }
//
//        if (isspace(current_char)) {
//            if (!is_space) {
//                // If a token just ended, null-terminate it and add to the array
//                line[token_length] = '\0';
//                tokens = (char **)realloc(tokens, (token_count + 1) * sizeof(char *));
//                tokens[token_count] = strdup(line);
//                token_count++;
//                token_length = 0;
//                is_space = 1;
//            }
//        } else {
//            // Append the character to the current token
//            line = (char *)realloc(line, line_length + 1);
//            line[token_length] = current_char;
//            token_length++;
//            line_length++;
//            is_space = 0;
//        }
//    }
//
//    // Check if there's a token at the end of the line
//    if (!is_space) {
//        line[token_length] = '\0';
//        tokens = (char **)realloc(tokens, (token_count + 1) * sizeof(char *));
//        tokens[token_count] = strdup(line);
//        token_count++;
//    }
//
//    *num_tokens = token_count;
//
//    if (token_count > 0) {
//        // Null-terminate the array of tokens
//        tokens = (char **)realloc(tokens, (token_count + 1) * sizeof(char *));
//        tokens[token_count] = NULL;
//    }
//
//    if (line_length > 0) {
//        free(line);
//    }
//
//    return tokens;
//}

