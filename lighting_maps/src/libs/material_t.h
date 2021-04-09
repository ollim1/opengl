#ifndef MATERIAL_T_H
#define MATERIAL_T_H
#include "../include/cglm/cglm.h"
#include "../include/cglm/call.h"
typedef struct material_t {
    unsigned diffuse; // texture unit id
    vec3 specular;
    float shininess;
} Material;

struct material_t *new_Material(unsigned diffuse, vec3 specular, float shininess);
void delete_Material(struct material_t *material);
#endif /* ifndef MATERIAL_T_H */
