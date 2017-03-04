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


void free_str_array(char** arr) {
    for (char** pLine = arr; *pLine != NULL; ++pLine) {
        free(*pLine);
    }
    free(arr);
}


char** div_format(char** s) {
    const size_t kResultBuffer = 128;

    char** result = NULL;
    size_t resultCount = 0;

    size_t divLevel = 0;
    bool divError = false;

    for (char** pLine = s; *pLine != NULL; ++pLine) {
        char* line = *pLine;

        char* lineStart = line;
        char* lineEnd = lineStart;

        bool isInTag = false;
        bool wasSlash = false;
        bool hadNonSpaceChar = false;
        bool wasOpenTag = false;
        bool wasCloseTag = false;

        for (char* pChar = line; *pChar != '\0'; ++pChar) {
            char c = *pChar;

            if (!isInTag && !hadNonSpaceChar && c != ' ') {
                hadNonSpaceChar = true;
                lineStart = pChar;
                lineEnd = lineStart;
            }

            switch (c) {
                case '<':
                    isInTag = true;
                    wasSlash = false;
                    hadNonSpaceChar = false;
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
                            if (divLevel == 0) {
                                divError = true;
                            } else {
                                divLevel--;
                                wasCloseTag = true;
                            }
                        } else {
                            divLevel++;
                            wasOpenTag = true;
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

            if (divError) {
                break;
            }

            if (wasOpenTag) {
                wasOpenTag = false;

                char* newLine = malloc(sizeof(char) * (7 + (divLevel - 1) * 4));
                for (size_t i = 0; i < (divLevel - 1) * 4; ++i) {
                    newLine[i] = ' ';
                }
                memcpy(newLine + (divLevel - 1) * 4, "<div>\n", 7);

                if (resultCount % kResultBuffer == 0) {
                    result = realloc(result, sizeof(char*) * (resultCount + kResultBuffer));
                }
                result[resultCount++] = newLine;
            }

            if (lineStart != lineEnd) {
                size_t strLen = lineEnd - lineStart;
                char* newLine = malloc(sizeof(char) * (strLen + 1 + divLevel * 4));
                for (size_t i = 0; i < divLevel * 4; ++i) {
                    newLine[i] = ' ';
                }
                memcpy(newLine + divLevel * 4, lineStart, sizeof(char) * strLen);
                newLine[strLen + divLevel * 4] = '\n';

                if (resultCount % kResultBuffer == 0) {
                    result = realloc(result, sizeof(char*) * (resultCount + kResultBuffer));
                }
                result[resultCount++] = newLine;

                lineStart = ++lineEnd;
            }

            if (wasCloseTag) {
                wasCloseTag = false;

                char* newLine = malloc(sizeof(char) * (8 + divLevel * 4));
                for (size_t i = 0; i < divLevel * 4; ++i) {
                    newLine[i] = ' ';
                }
                memcpy(newLine + divLevel * 4, "</div>\n", 8);

                if (resultCount % kResultBuffer == 0) {
                    result = realloc(result, sizeof(char*) * (resultCount + kResultBuffer));
                }
                result[resultCount++] = newLine;
            }
        }

        if (divError) {
            break;
        }
    }

    if (divLevel != 0) {
        divError = true;
    }

    if (divError) {
        free_str_array(result);
        result = malloc(sizeof(char*));
        result[0] = malloc(sizeof(char) * 8);
        memcpy(result[0], "[error]", 8);
        resultCount = 1;
    }

    if (result == NULL) {
        result = malloc(sizeof(char*));
    } else if (resultCount % kResultBuffer == 0) {
        result = realloc(result, sizeof(char*) * (resultCount + 1));
    }
    result[resultCount] = NULL;

    return result;
}


int main() {
    char** input = read_all_stdin();
    char** output = div_format(input);
    write_lines(output);
    free_str_array(input);
    free_str_array(output);
    return 0;
}