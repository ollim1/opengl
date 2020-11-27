#include    "fileload.h"
#include    <stdio.h>
#include    <stdlib.h>

char *loadToStr(char *filename)
{
    if (!filename)
        return NULL;

    FILE *fptr;

    size_t len = 4096;
    int count = 0;
    char *str = calloc(len, sizeof(char));
    
    fptr = fopen(filename, "r");
    if (!fptr) {
        printf("failed to load file %s\n", filename);
        return NULL;
    }
    
    char c;
    while ((c=fgetc(fptr)) != EOF) {
        if (count >= len - 2) {
            len *= 2;
            char *temp = realloc(str, len);
            if (!temp) {
                printf("failed to realloc (%ld) while reading %s\n", len, filename);
                fclose(fptr);
                return NULL;
            }
            str = temp;
        }
        str[count] = c;
        count++;
    }

    if (count == 0)
        puts("nothing was read");
    str[count] = '\0';
    fclose(fptr);

    return str;
}
