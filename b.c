#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>
#include <assert.h>


typedef char bool;
#define true 1
#define false 0


typedef struct {
    int* data;
    size_t count;
    size_t capacity;
    size_t growSize;
} int_array;

void int_array_construct(int_array* pArray) {
    pArray->growSize = 128;
    pArray->capacity = pArray->growSize;
    pArray->count = 0;
    pArray->data = malloc(sizeof(int) * pArray->capacity);
}

void int_array_destruct(int_array* pArray) {
    free(pArray->data);
}

void int_array_grow(int_array* pArray) {
    pArray->capacity += pArray->growSize;
    pArray->data = realloc(pArray->data, sizeof(char) * pArray->capacity);
}

void int_array_push(int_array* pArray, int object) {
    if (pArray->count == pArray->capacity) {
        int_array_grow(pArray);
    }
    pArray->data[pArray->count++] = object;
}

int int_array_pop(int_array* pArray) {
    assert(pArray->count > 0);
    return pArray->data[--pArray->count];
}


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


typedef struct {
    char* name;
    bool value;
} var_def;

void var_def_destruct(var_def* pVarDef) {
    free(pVarDef->name);
}


typedef struct {
    var_def* data;
    size_t count;
    size_t capacity;
    size_t growSize;
} var_def_array;

void var_def_array_construct(var_def_array* pArray) {
    pArray->growSize = 128;
    pArray->capacity = pArray->growSize;
    pArray->count = 0;
    pArray->data = malloc(sizeof(var_def) * pArray->capacity);
}

void var_def_array_destruct(var_def_array* pArray) {
    for (size_t i = 0; i < pArray->count; i++) {
        var_def_destruct(pArray[i].data);
    }
    free(pArray->data);
}

void var_def_array_grow(var_def_array* pArray) {
    pArray->capacity += pArray->growSize;
    pArray->data = realloc(pArray->data, sizeof(var_def) * pArray->capacity);
}

void var_def_array_push(var_def_array* pArray, var_def object) {
    if (pArray->count == pArray->capacity) {
        var_def_array_grow(pArray);
    }
    pArray->data[pArray->count++] = object;
}

int var_def_array_find(var_def_array* pArray, char* name) {
    for (size_t i = 0; i < pArray->count; i++) {
        char* varName = pArray[i].data->name;
        size_t varNameLen = strlen(varName);
        char* nameEnd = name + varNameLen;
        int varValue = pArray[i].data->value;
        if (memcmp(varName, name, strlen(varName)) == 0 && (*nameEnd == '\0' || *nameEnd == ' ')) {
            return varValue;
        }
    }
    return -1;
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


char* create_polksi_notation(char* infixNotation, var_def_array* pVarDefs) {
    growable_string polskiNotation;
    growable_string_construct(&polskiNotation);

    string_array operations;
    string_array_construct(&operations);

    for (char* pChar = infixNotation; *pChar != '\0'; pChar++) {
        char c = *pChar;
        if (c == '(') {
            string_array_push_copy(&operations, "(");
        } else if (c == ')') {
            while (operations.count > 0) {
                char* op = string_array_pop(&operations);
                if (op[0] == '(') {
                    free(op);
                    break;
                }
                growable_string_append_string(&polskiNotation, op);
                free(op);
            }
        } else if (memcmp(pChar, "and", 3)) {
            string_array_push_copy(&operations, "*");
        } else if (memcmp(pChar, "xor", 3)) {
            if (operations.count > 0 && string_array_get_last(&operations)[0] == '*') {
                string_array_delete_last(&operations);
                growable_string_append_char(&polskiNotation, '*');
            }
            string_array_push_copy(&operations, "^");
        } else if (memcmp(pChar, "or", 2)) {
            if (operations.count > 0 && string_array_get_last(&operations)[0] == '*') {
                string_array_delete_last(&operations);
                growable_string_append_char(&polskiNotation, '*');
            }
            if (operations.count > 0 && string_array_get_last(&operations)[0] == '^') {
                string_array_delete_last(&operations);
                growable_string_append_char(&polskiNotation, '^');
            }
            string_array_push_copy(&operations, "+");
        } else if (memcmp(pChar, "not", 2)) {
            string_array_push_copy(&operations, "-");
        } else {
            switch (var_def_array_find(pVarDefs, pChar)) {
                case 0:
                    growable_string_append_char(&polskiNotation, '0');
                    break;

                case 1:
                    growable_string_append_char(&polskiNotation, '1');
                    break;

                default:
                    break;
            }
            break;
        }
    }

    while (operations.count > 0) {
        char* op = string_array_pop(&operations);
        growable_string_append_string(&polskiNotation, op);
        free(op);
    }

    string_array_destruct(&operations);

    return polskiNotation.data;
}

int calculate_polski_notation(char* polskiNotation, var_def_array* pVarDefs) {
    int_array stack;
    int_array_construct(&stack);
    for (char* pChar = polskiNotation; *pChar != '\0'; pChar++) {
        int a, b, c;
        switch (*pChar) {
            case '0':
                int_array_push(&stack, 0);
                break;

            case '1':
                int_array_push(&stack, 0);
                break;

            case '*':
                a = int_array_pop(&stack);
                b = int_array_pop(&stack);
                int_array_push(&stack, a * b);
                break;

            case '+':
                a = int_array_pop(&stack);
                b = int_array_pop(&stack);
                c = a + b;
                if (c > 1) {
                    c = 1;
                }
                int_array_push(&stack, c);
                break;

            case '^':
                a = int_array_pop(&stack);
                b = int_array_pop(&stack);
                c = a + b;
                if (c == 2) {
                    c = 1;
                } else {
                    c = 0;
                }
                int_array_push(&stack, c);
                break;

            case '-':
                // TODO
                break;
        }
    }
    int result = stack.data[0];
    int_array_destruct(&stack);
    return result;
}


#define EVALUATE__PARSE_VAR_DEF__OK 0
#define EVALUATE__PARSE_VAR_DEF__NOT_VAR 1
#define EVALUATE__PARSE_VAR_DEF__PARSE_ERROR 2

int evaluate__parse_var_def(char* line, var_def_array* pArray) {
    var_def newVarDef;

    char* equalSign = strchr(line, '=');
    if (equalSign == NULL) {
        return EVALUATE__PARSE_VAR_DEF__NOT_VAR;
    }
    char* semicolon = strchr(equalSign + 1, ';');
    if (semicolon == NULL) {
        return EVALUATE__PARSE_VAR_DEF__NOT_VAR;
    }

    // Key parsing
    char* keyStart;
    for (keyStart = line; *keyStart == ' '; keyStart++); // skip spaces

    char* keyEnd;
    for (keyEnd = equalSign - 1; *keyEnd == ' '; keyEnd--); // move backwards while there're spaces
    keyEnd++; // end is exclusive

    size_t keyLen = keyEnd - keyStart;
    newVarDef.name = malloc(sizeof(char) * (keyLen + 1));
    memcpy(newVarDef.name, keyStart, keyLen);
    newVarDef.name[keyLen] = '\0';

    for (char* pChar = newVarDef.name; *pChar != '\0'; pChar++) {
        if (!islower(*pChar)) {
            return EVALUATE__PARSE_VAR_DEF__PARSE_ERROR;
        }
    }
    if (strcmp(newVarDef.name, "not") == 0) {
        return EVALUATE__PARSE_VAR_DEF__PARSE_ERROR;
    }
    if (strcmp(newVarDef.name, "and") == 0) {
        return EVALUATE__PARSE_VAR_DEF__PARSE_ERROR;
    }
    if (strcmp(newVarDef.name, "or") == 0) {
        return EVALUATE__PARSE_VAR_DEF__PARSE_ERROR;
    }

    // Value parsing
    char* valueStart;
    for (valueStart = equalSign + 1; *valueStart == ' '; valueStart++); // skip spaces

    if (memcmp(valueStart, "True", 4) == 0) {
        if (valueStart[4] != ' ' && valueStart[4] != '\0') {
            return EVALUATE__PARSE_VAR_DEF__PARSE_ERROR;
        }
        newVarDef.value = 1;
    } else if (memcmp(valueStart, "False", 5) == 0) {
        if (valueStart[5] != ' ' && valueStart[5] != '\0') {
            return EVALUATE__PARSE_VAR_DEF__PARSE_ERROR;
        }
        newVarDef.value = 0;
    }

    // Ok
    var_def_array_push(pArray, newVarDef);
    return EVALUATE__PARSE_VAR_DEF__OK;
}

int evaluate__parse_question(char* line, var_def_array* pVarDefs) {
    char* polskiNotation = create_polksi_notation(line, pVarDefs);
    int result = calculate_polski_notation(polskiNotation, pVarDefs);
    free(polskiNotation);
    return result;
}

int evaluate(char** code) {
    var_def_array var_defs;
    var_def_array_construct(&var_defs);

    for (char** pLine = code; *pLine != NULL; ++pLine) {
        char* line = *pLine;

        int parseVarDefCode = evaluate__parse_var_def(line, &var_defs);
        if (parseVarDefCode == EVALUATE__PARSE_VAR_DEF__NOT_VAR) {
            evaluate__parse_question(line, &var_defs);
        }
    }

    var_def_array_destruct(&var_defs);
}


int main() {
    char** input = read_all_stdin();
    printf("%d\n", evaluate(input));
    free_str_array(input);
    return 0;
}