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