#ifndef MATERIAL_T_H
#define MATERIAL_T_H
#include "include/cglm/cglm.h"
#include "include/cglm/call.h"
typedef struct material_t {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
} Material;

struct material_t *new_Material(vec3 ambient, vec3 diffuse, vec3 specular, float shininess);
void delete_Material(struct material_t *material);
#endif /* ifndef MATERIAL_T_H */
