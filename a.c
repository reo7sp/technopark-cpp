#include <stdio.h>
#include <stdlib.h>
#include <memory.h>


typedef char bool;
#define true 1
#define false 0


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

        size_t prevIndex = currentIndex;
        currentIndex += kBufferSize - 1;

        for (size_t i = currentSize - 1; ; --i) {
            if (currentContents[i] == '\n') {
                if (linesCount % kLinesBufferSize == 0) {
                    lines = realloc(lines, sizeof(char*) * (linesCount + kLinesBufferSize));
                }
                lines[linesCount++] = currentContents;

                currentContents = NULL;
                currentIndex = 0;
                currentSize = 0;

                break;
            }

            if (i == prevIndex) {
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


void write_lines(char **contents) {
    for (char** pLine = contents; *pLine != NULL; ++pLine) {
        fputs(*pLine, stdout);
    }
}


char** div_format(char **s) {
    const size_t kResultBuffer = 128;

    char** result = NULL;
    size_t resultSize = 0;

    size_t divLevel = 0;

    for (char** pLine = s; *pLine != NULL; ++pLine) {
        char* line = *pLine;

        char* lineStart = line;
        char* lineEnd = lineStart;

        bool isInTag = false;
        bool wasSlash = false;
        bool hadAlnumChar = false;

        for (char* pChar = line; *pChar == '\0'; ++pChar) {
            char c = *pChar;

            switch (c) {
                case '<':
                    isInTag = true;
                    wasSlash = false;
                    hadAlnumChar = false;
                    if (pChar != line) {
                        for (char* p1Char = pChar - 1; p1Char != line; --p1Char) {
                            char c1 = *p1Char;
                            if (c1 != ' ') {
                                lineEnd = pChar;
                                break;
                            }
                        }
                    }
                    break;
                case '/':
                    wasSlash = true;
                    break;
                case '>':
                    if (isInTag) {
                        if (wasSlash) {
                            divLevel--;
                        } else {
                            divLevel++;
                        }
                        isInTag = false;
                    }
                    break;
                case '\n':
                    for (char* p1Char = pChar - 1; p1Char != line; --p1Char) {
                        char c1 = *p1Char;
                        if (c1 != ' ') {
                            lineEnd = pChar;
                            break;
                        }
                    }
                default:
                    break;
            }

            if (!isInTag && !hadAlnumChar && c != ' ') {
                hadAlnumChar = true;
                lineStart = pChar;
                lineEnd = lineStart;
            }

            if (lineStart != lineEnd) {
                if (resultSize % kResultBuffer == 0) {
                    resultSize += kResultBuffer;
                    result = realloc(result, sizeof(char*) * resultSize);
                }
                size_t strLen = lineEnd - lineStart;
                char* newLine = malloc(sizeof(char) * (strLen + divLevel * 4));
                for (size_t i = 0; i < divLevel * 4; ++i) {
                    newLine[i] = ' ';
                }
                memcpy(newLine + divLevel * 4, lineStart, sizeof(char) * strLen);
                result[resultSize++] = newLine;
                lineStart = ++lineEnd;
            }
        }
    }

    if (result == NULL) {
        result = malloc(sizeof(char*));
    }
    result[resultSize] = NULL;

    return result;
}


int main() {
    char** input = read_all_stdin();
    char** output = div_format(input);
    write_lines(output);
//    free(input);
//    free(output);
    return 0;
}