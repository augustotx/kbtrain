#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>

void enableRawMode(struct termios *orig) {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    *orig = raw;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disableRawMode(struct termios *orig) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, orig);
}

char *generateRandomString(size_t length) {
    static const char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
    char *randomString = malloc(length + 1);
    if (!randomString) return NULL;

    for (size_t i = 0; i < length; i++) {
        randomString[i] = alphabet[rand() % (sizeof(alphabet) - 1)];
    }
    randomString[length] = '\0';
    return randomString;
}

int main() {
    unsigned long starttime = time(NULL);
    int wordcount = 0;
    double total, totalcorrect, totalincorrect;
    total = 0;
    totalincorrect = 0;
    totalcorrect = 0;
    srand(time(NULL));
    struct termios original;
    enableRawMode(&original);

    while (1) {
        int wordLength = rand() % 6 + 5;
        char *chosenWord = generateRandomString(wordLength);
        if (!chosenWord) {
            fprintf(stderr, "Failed to generate random word\n");
            continue;
        }
        int correct = 1;
        printf("\033[2K"); // Clear the line
        printf("\r%s", chosenWord); // Print the word at the start of the line
        printf("\033[%luD", strlen(chosenWord)); // Move cursor left to the start of the word
        char ch;
        int exiter = 0;
        for (size_t i = 0; i < strlen(chosenWord); ++i) {
            ch = getchar();
            if (ch == '0') {
                free(chosenWord);
                exiter = 1;
                break;
            }
        if (ch != chosenWord[i]) {
                correct = 0;
                totalincorrect+=1;
                printf("\033[31m%c\033[0m", ch); // Red for incorrect
            } else {
                totalcorrect+=1;
                printf("\033[32m%c\033[0m", ch); // Green for correct
            }
        }

        if (exiter){
            total = totalcorrect + totalincorrect;
            total = totalcorrect/total;
            total *= 100;
            break;
        } else {
            wordcount++;
        }
    }

    disableRawMode(&original);
    unsigned long exittime = time(NULL);
    unsigned long totaltime = exittime - starttime;
    double wpm = (double)wordcount / ((double)totaltime / 60);
    printf("\nExiting.\nTotal correct: %.2f%%\nWords per minute: %.2f\nTest time: %ds\nCompleted words: %d",total,wpm,(int)totaltime, wordcount);

    return 0;
}
