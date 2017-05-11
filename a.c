#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>


typedef char bool;
#define true 1
#define false 0


typedef struct {
    char* data;
    size_t length;
    size_t capacity;
    size_t growSize;
} growable_string;

void growable_string_construct(growable_string* pString) {
    pString->growSize = 128;
    pString->capacity = pString->growSize;
    pString->length = 0;
    pString->data = malloc(sizeof(char) * pString->capacity);
    pString->data[0] = '\0';
}

void growable_string_destruct(growable_string* pString) {
    free(pString->data);
}

void growable_string_grow(growable_string* pString) {
    pString->capacity += pString->growSize;
    pString->data = realloc(pString->data, sizeof(char) * pString->capacity);
}

void growable_string_append_char(growable_string* pString, char c) {
    if (pString->length == pString->capacity - 1) {
        growable_string_grow(pString);
    }
    pString->data[pString->length++] = c;
    pString->data[pString->length] = '\0';
}

void growable_string_append_string(growable_string* pString, char* str) {
    for (char* pChar = str; *pChar != '\0'; pChar++) {
        growable_string_append_char(pString, *pChar);
    }
}

char* growable_string_get_end(growable_string* pString) {
    return pString->data + pString->length;
}


typedef struct {
    char** data;
    size_t count;
    size_t capacity;
    size_t growSize;
} string_array;

void string_array_construct(string_array* pArray) {
    pArray->growSize = 128;
    pArray->capacity = pArray->growSize;
    pArray->count = 0;
    pArray->data = malloc(sizeof(char*) * pArray->capacity);
    pArray->data[0] = NULL;
}

void string_array_destruct(string_array* pArray) {
    for (size_t i = 0; i < pArray->count; i++) {
        free(pArray->data + i);
    }
    free(pArray->data);
}

void string_array_shallow_destruct(string_array* pArray) {
    free(pArray->data);
}

void string_array_grow(string_array* pArray) {
    pArray->capacity += pArray->growSize;
    pArray->data = realloc(pArray->data, sizeof(char*) * pArray->capacity);
}

void string_array_push(string_array* pArray, char* object) {
    if (pArray->count == pArray->capacity - 1) {
        string_array_grow(pArray);
    }
    pArray->data[pArray->count++] = object;
    pArray->data[pArray->count] = NULL;
}

void string_array_push_copy(string_array* pArray, char* object) {
    char* copy = malloc(sizeof(char) * strlen(object));
    strcpy(copy, object);
    string_array_push(pArray, copy);
}

char* string_array_get_last(string_array* pArray) {
    assert(pArray->count > 0);
    return pArray->data[--pArray->count];
}

char* string_array_pop(string_array* pArray) {
    assert(pArray->count > 0);
    return pArray->data[--pArray->count];
}

void string_array_delete_last(string_array* pArray) {
    free(string_array_pop(pArray));
}


char** read_all_stdin() {
    string_array result;
    string_array_construct(&result);

    growable_string currentLine;
    growable_string_construct(&currentLine);

    while (!feof(stdin)) {
        growable_string_grow(&currentLine);

        char* strEnd = growable_string_get_end(&currentLine);
        int readCount = (int) currentLine.growSize;
        fgets(strEnd, readCount, stdin);
        currentLine.length += readCount;

        char* oldStrEnd = strEnd;
        strEnd = growable_string_get_end(&currentLine);

        for (char* pChar = strEnd; oldStrEnd <= pChar && pChar <= strEnd; pChar--) {
            if (*pChar == '\n') {
                string_array_push(&result, currentLine.data);

                growable_string_construct(&currentLine);
                break;
            }
        }
    }

    return result.data;
}

void write_lines(char** contents) {
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
    string_array result;
    string_array_construct(&result);

    size_t divLevel = 0;
    bool divError = false;

    for (char** pLine = s; *pLine != NULL; ++pLine) {
        char* line = *pLine;

        char* newLineContentsStart = line;
        char* newLineContentsEnd = newLineContentsStart;

        bool isInTag = false;
        bool wasSlash = false;
        bool hadNonSpaceChar = false;
        bool wasOpenTag = false;
        bool wasCloseTag = false;

        for (char* pChar = line; *pChar != '\0'; ++pChar) {
            char c = *pChar;

            if (!isInTag && !hadNonSpaceChar) {
                if (c != ' ') {
                    hadNonSpaceChar = true;
                    newLineContentsStart = pChar;
                    newLineContentsEnd = pChar;
                }
            }

            switch (c) {
                case '<': {
                    isInTag = true;
                    wasSlash = false;
                    hadNonSpaceChar = false;

                    const bool isFirstCharInLine = pChar != line;
                    if (isFirstCharInLine) {
                        char* nonSpace = NULL;
                        for (char* p1Char = pChar - 1; p1Char != line; --p1Char) {
                            char c1 = *p1Char;
                            if (c1 != ' ') {
                                nonSpace = pChar;
                                break;
                            }
                        }
                        if (nonSpace != NULL) {
                            newLineContentsEnd = nonSpace;
                        }
                    }
                    break;
                }

                case '/': {
                    wasSlash = true;
                    break;
                }

                case '>': {
                    if (isInTag) {
                        isInTag = false;
                        if (wasSlash) {
                            if (divLevel == 0) {
                                divError = true;
                            } else {
                                wasCloseTag = true;
                                divLevel--;
                            }
                        } else {
                            wasOpenTag = true;
                            divLevel++;
                        }
                    }
                    break;
                }

                case '\n': {
                    char* nonSpace = NULL;
                    for (char* p1Char = pChar - 1; p1Char != line; --p1Char) {
                        char c1 = *p1Char;
                        if (c1 != ' ') {
                            nonSpace = pChar;
                            break;
                        }
                    }
                    if (nonSpace != NULL) {
                        newLineContentsEnd = nonSpace;
                    }
                    break;
                }

                default:
                    break;
            }

            if (divError) {
                break;
            }

            if (wasOpenTag) {
                wasOpenTag = false;

                char* divString = "<div>\n";
                size_t indentSpacesCount = (divLevel - 1) * 4;

                size_t newLineLen = strlen(divString) + indentSpacesCount;
                char* newLine = malloc(sizeof(char) * (newLineLen + 1));
                memset(newLine, ' ', sizeof(char) * indentSpacesCount);
                memcpy(newLine + indentSpacesCount, divString, sizeof(char) * strlen(divString));
                newLine[newLineLen] = '\0';
                string_array_push(&result, newLine);
            }

            if (newLineContentsStart != newLineContentsEnd) {
                size_t contentsLen = newLineContentsEnd - newLineContentsStart;
                size_t indentSpacesCount = divLevel * 4;

                size_t newLineLen = contentsLen + indentSpacesCount;
                char* newLine = malloc(sizeof(char) * (newLineLen + 1));
                memset(newLine, ' ', sizeof(char) * indentSpacesCount);
                memcpy(newLine + indentSpacesCount, newLineContentsStart, sizeof(char) * contentsLen);
                newLine[newLineLen - 1] = '\n';
                newLine[newLineLen] = '\0';
                string_array_push(&result, newLine);

                newLineContentsStart = ++newLineContentsEnd;
            }

            if (wasCloseTag) {
                wasCloseTag = false;

                char* divString = "</div>\n";
                size_t indentSpacesCount = divLevel * 4;

                size_t newLineLen = strlen(divString) + indentSpacesCount;
                char* newLine = malloc(sizeof(char) * (newLineLen + 1));
                memset(newLine, ' ', sizeof(char) * indentSpacesCount);
                memcpy(newLine + indentSpacesCount, divString, sizeof(char) * strlen(divString));
                newLine[newLineLen] = '\0';
                string_array_push(&result, newLine);
            }
        }

        if (divError) {
            break;
        }
    }

    if (divLevel != 0) {
        string_array_destruct(&result);
        string_array_construct(&result);
        string_array_push_copy(&result, "[error]");
    }

    return result.data;
}


int main() {
    char** input = read_all_stdin();
    char** output = div_format(input);
    write_lines(output);
    free_str_array(input);
    free_str_array(output);
    return 0;
}