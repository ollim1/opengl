#include "strbuild_t.h"
#include <string.h>

struct strbuild_t *new_StringBuilder(char *str)
{
    if (!str)
        return NULL;

    struct strbuild_t *strbuild = malloc(sizeof(struct strbuild_t));
    if (!strbuild) {
        return NULL;
    }
    size_t len = strlen(str);
    strbuild->str = malloc(len + 1);
    strcpy(strbuild->str, str);
    
    strbuild->length = len;
    strbuild->size = len + 1;

    return strbuild;
}

void stringBuilder_append(struct strbuild_t *strbuild, char *str)
{
    if (!strbuild || !str) {
        return;
    }

    strbuild->length += strlen(str);
    strbuild->size = strbuild->length + 1;
    char *temp = realloc(strbuild->str, strbuild->size);
    if (!temp) {
        puts("failed to realloc");
        return;
    }

    strbuild->str = temp;
    strcat(strbuild->str, str);
}

void delete_StringBuilder(struct strbuild_t *strbuild)
{
    if (!strbuild || !strbuild->str) {
        return;
    }

    free(strbuild->str);
    free(strbuild);
}

char *stringBuilder_getStr(struct strbuild_t *strbuild)
{
    if (!strbuild || !strbuild->str)
        return NULL;

    char *str = calloc(strlen(strbuild->str) + 1, sizeof(char));
    if (!str)
        return NULL;

    strcpy(str, strbuild->str);
    return str;
}
