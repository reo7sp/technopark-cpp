#include <stdio.h>
#include <stdlib.h>

char** read_all_stdin() {
    const size_t kBufferSize = 128;
    const size_t kLinesBufferSize = 128;

    char** lines = NULL;
    size_t linesCount = 0;

    char* currentContents = NULL;
    size_t currentIndex = 0;
    size_t currentSize = 0;

    while (!feof(stdin)) {
        currentSize += kBufferSize;
        currentContents = realloc(currentContents, sizeof(char) * currentSize);
        fgets(currentContents + currentIndex, kBufferSize, stdin);

        for (size_t i = currentSize - 1; ; --i) {
            if (currentContents[i] == '\n') {
                if (linesCount % kLinesBufferSize == 0) {
                    lines = realloc(lines, sizeof(char*) * (linesCount + kLinesBufferSize));
                }
                lines[linesCount++] = currentContents;
                currentContents = NULL;
                currentSize = 0;
                break;
            }
            if (i == currentIndex) {
                break;
            }
        }

        currentIndex += kBufferSize - 1;
    }

    if (linesCount == 0) {
        lines = malloc(sizeof(char*));
    } else if (linesCount % kLinesBufferSize == 0) {
        lines = realloc(lines, sizeof(char*) * (linesCount + 1));
    }
    lines[linesCount] = NULL;

    return lines;
}

void write_lines(char **contents) {
    for (char** pLine = contents; *pLine != NULL; ++pLine) {
        fputs(*pLine, stdout);
    }
}

char** div_format(char **s) {
    return s;
}

int main() {
    char** input = read_all_stdin();
    char** output = div_format(input);
    write_lines(output);
    free(input);
//    free(output);
    return 0;
}