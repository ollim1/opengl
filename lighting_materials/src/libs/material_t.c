#include "material_t.h"
#include <string.h>

struct material_t *new_Material(vec3 ambient, vec3 diffuse, vec3 specular, float shininess)
{
    struct material_t *material = malloc(sizeof(struct material_t));
    if (!material)
        return NULL;

    memcpy(material->ambient, ambient, sizeof(vec3));
    memcpy(material->diffuse, diffuse, sizeof(vec3));
    memcpy(material->specular, specular, sizeof(vec3));
    material->shininess = shininess;

    return material;
}
void delete_Material(struct material_t *material)
{
    if (!material)
        return;

    free(material);
}
