#include "light_t.h"
#include <string.h>

struct light_t *new_Light(vec3 position, vec3 ambient, vec3 diffuse, vec3 specular)
{
    struct light_t *light = malloc(sizeof(struct light_t));
    if (!light)
        return NULL;

    memcpy(light->position, position, sizeof(vec3));
    memcpy(light->ambient, ambient, sizeof(vec3));
    memcpy(light->diffuse, diffuse, sizeof(vec3));
    memcpy(light->specular, specular, sizeof(vec3));

    return light;
}
void delete_Light(struct light_t *light)
{
    if (!light)
        return;

    free(light);
}
