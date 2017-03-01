#include <stdio.h>
#include <stdlib.h>

char** readAllStdin() {
    const size_t kBufferSize = 128;
    const size_t kLinesBufferSize = 128;

    size_t linesCount = 0;
    char** lines = NULL;

    char* currentContents = NULL;

    while (!feof(stdin)) {
        currentContents = realloc(currentContents, sizeof(char) * kBufferSize);
        fgets(currentContents, kBufferSize, stdin);

        for (size_t i = kBufferSize - 1; ; --i) {
            if (currentContents[i] == '\n') {
                if (linesCount % kLinesBufferSize == 0) {
                    lines = realloc(lines, sizeof(char*) * (linesCount + kLinesBufferSize));
                }
                lines[linesCount++] = currentContents;
                currentContents = NULL;
                break;
            }
            if (i == 0) {
                break;
            }
        }
    }

    if (linesCount == 0) {
        lines = malloc(sizeof(char*));
    } else if (linesCount % kLinesBufferSize == 0) {
        lines = realloc(lines, sizeof(char*) * (linesCount + 1));
    }
    lines[linesCount] = NULL;

    return lines;
}

void writeLines(char** contents) {
    for (char** pLine = contents; *pLine != NULL; ++pLine) {
        fputs(*pLine, stdout);
    }
}

char** div_format(char **s) {
    return s;
}

int main() {
    char** input = readAllStdin();
    char** output = div_format(input);
    writeLines(output);
    return 0;
}