#ifndef LIGHT_T_H
#define LIGHT_T_H
#include "../include/cglm/cglm.h"
#include "../include/cglm/call.h"
typedef struct light_t {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;
} Light;

struct light_t *new_Light(vec3 position, vec3 direction, vec3 ambient, vec3 diffuse, vec3 specular, float constant, float linear, float quadratic, float cutOff, float outerCutOff);
void delete_Light(struct light_t *light);
#endif /* ifndef LIGHT_T_H */
