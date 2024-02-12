#include "mio.h"

struct WordNode {
    char* word;
    int count;
    struct WordNode* next;
};

struct WordList {
    struct WordNode* head;
};

// Function to find a word in the word list and return its node
struct WordNode* findWord(struct WordList* list, const char* word) {
    struct WordNode* current = list->head;
    while (current != NULL) {
        if (strcmp(current->word, word) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void insertWord(struct WordList* list, const char* word) {
    struct WordNode* existingNode = findWord(list, word);
    if (existingNode != NULL) {
        existingNode->count++;
    } else {
        struct WordNode* newNode = (struct WordNode*)malloc(sizeof(struct WordNode));
        newNode->word = strdup(word);
        newNode->count = 1;
        newNode->next = list->head;
        list->head = newNode;
    }
}

void freeWordList(struct WordList* list) {
    struct WordNode* current = list->head;
    while (current != NULL) {
        struct WordNode* temp = current;
        current = current->next;
        free(temp->word);
        free(temp);
    }
}

int main(int argc, char* argv[]) {
    minit(); // Initialize MIO

    struct WordList wordList = { .head = NULL };
    int totalWords = 0;

    while (1) {
        int length;
        char* word = mgets(mtdin, &length);
        if (word == NULL)
            break;

        insertWord(&wordList, word);
        totalWords++;

        struct WordNode* current = findWord(&wordList, word);
        mputi(mtdout, current->count);
        mputc(mtdout, ',');
        mputc(mtdout, ' ');
        mputs(mtdout, word, length);
        mputc(mtdout, '\n');

        // Flush the output buffer to ensure immediate display
        mflush(mtdout);
    }

    mputc(mtdout, '\n');

    struct WordNode* current = wordList.head;
    while (current != NULL) {
        mputs(mtdout, current->word, strlen(current->word));
        mputc(mtdout, ':');
        mputc(mtdout, ' ');
        mputi(mtdout, current->count);
        mputc(mtdout, '\n');
        current = current->next;

        // Flush the output buffer to ensure immediate display
        mflush(mtdout);
    }

    mputc(mtdout, '\n');

    const char* message = "Total WordCount = ";
    mputs(mtdout, message, strlen(message));
    mputi(mtdout, totalWords);
    mputc(mtdout, '\n');

    freeWordList(&wordList);

    return 0;
}
