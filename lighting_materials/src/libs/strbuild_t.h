#ifndef STRBUILD_T_H
#define STRBUILD_T_H
#include <stdlib.h>

typedef struct strbuild_t {
    size_t length; // length of string
    size_t size; // size of allocated array
    char *str;
} StringBuilder;

struct strbuild_t *new_StringBuilder(char *str);
void stringBuilder_append(struct strbuild_t *strbuild, char *str);
char *stringBuilder_getStr(struct strbuild_t *strbuild);
void delete_StringBuilder(struct strbuild_t *strbuild);
#endif /* ifndef STRBUILD_T_H */
