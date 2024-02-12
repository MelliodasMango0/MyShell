#include "mio.h"

// Function to compare two strings case-insensitively
int strcasecmp(const char* str1, const char* str2) {
    while (*str1 != '\0' && *str2 != '\0') {
        if (tolower(*str1) != tolower(*str2)) {
            return 0; // Strings are different
        }
        str1++;
        str2++;
    }
    return (*str1 == '\0' && *str2 == '\0'); // Strings are equal if both are null-terminated
}

// Function to search for a word in an array of words and return its index
int find_word_index(const char* word, const char* word_array[], int word_count) {
    for (int i = 0; i < word_count; i++) {
        if (strcasecmp(word, word_array[i])) {
            return i; // Word found, return its index
        }
    }
    return -1; // Word not found
}

int main(int argc, char* argv[]) {
    // Initialize custom I/O streams
    minit();

    // Check if the correct number of arguments is provided
    if (argc != 2) {
        const char* error_message = "Provide a single filename as an argument.\n";
        mwrite(mtderr, error_message, (int)strlen(error_message));
        return 1;
    }

    // open the word replacement file given in the argument
    MILE* my_file = mopen(argv[1], MODE_R, MBSIZE);
    if (my_file == NULL) {
        const char* err_message = "Error opening the provided file.\n";
        mwrite(mtderr, err_message, (int)strlen(err_message));
        return -1;
    }

    char** targets = NULL;       // initialize arrays of strings
    char** replacements = NULL;
    int target_count = 0;        // counters to keep track of # of elements in the array
    int replacement_count = 0;
    int pairs_count = 0;

    // loop to read replacements text file into two arrays
    while (1) {
        char* target_str;
        int length;
        int new_size;

        target_str = mgets(my_file, &length);              // read in the target string
        if (target_str == NULL) break;                    // if EOF, break
        new_size = sizeof(char*) * (target_count + 1);    // calculate new size of array to accommodate string
        targets = (char**)realloc((void*)targets, new_size);       // expand the array
        targets[target_count] = target_str;                 // add the new string to the end of the array
        target_count++;                                  // increment # of elements in array

        char* replacement_str = mgets(my_file, &length);              // repeat for the replacement string
        if (replacement_str == NULL) break;
        new_size = sizeof(char*) * (replacement_count + 1);
        replacements = (char**)realloc((void*)replacements, new_size);
        replacements[replacement_count] = replacement_str;
        replacement_count++;

        pairs_count++;                                    // increment # of target/replacement pairs
    }

    mclose(my_file);

    int length = 0;
    char* input_str;

    while (1) {
        input_str = mgets(mtdin, &length);   // get a string from standard in
        if (input_str == NULL) break;        // loop repeats until EOF (mgets returns NULL on EOF)

        for (int i = 0; i < length; i++) {  // make every character in the string lowercase
            char* ch = &(input_str[i]);
            if ((*ch >= 'A') && (*ch <= 'Z'))
                *ch = *ch - 'A' + 'a';
        }

        // remove punctuation
        char* new_str = (char*)malloc(strlen(input_str));
        int stripped_length = 0;
        for (int i = 0; i < length; i++) {
            char input_char = input_str[i];
            if ((input_char >= 33 && input_char <= 47) || (input_char >= 58 && input_char <= 64) || (input_char >= 123 && input_char <= 126)) {
                continue;
            }
            new_str[stripped_length] = input_char;
            stripped_length++;
        }

        new_str = (char*)realloc(new_str, stripped_length * sizeof(char));

        int i = find_word_index(new_str, targets, target_count);
        if (i != -1) {
            mputs(mtdout, replacements[i], (int)strlen(replacements[i]));
        } else {
            mputs(mtdout, new_str, stripped_length);
        }
        mputc(mtdout, ' ');

        free(new_str);
    }

    // Free all strings in both arrays before ending program
    for (int i = 0; i < target_count; i++) {
        free(targets[i]);
    }
    free(targets);

    for (int i = 0; i < replacement_count; i++) {
        free(replacements[i]);
    }
    free(replacements);

    return 0;
}
