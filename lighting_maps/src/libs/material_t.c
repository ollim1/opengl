#include "material_t.h"
#include <string.h>

struct material_t *new_Material(unsigned diffuse, vec3 specular, float shininess)
{
    struct material_t *material = malloc(sizeof(struct material_t));
    if (!material)
        return NULL;

    material->diffuse = diffuse;
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
