#ifndef STRUCT_H
#define STRUCT_H value
#include "material_t.h"
#include "light_t.h"
typedef struct shader_t {
    unsigned id;
} Shader;

struct shader_t *new_Shader(char *vertexShaderSource, char *fragmentShaderSource);
void delete_Shader(struct shader_t *shader);
void shader_use(struct shader_t *shader);
void shader_setBool(struct shader_t *shader, char *name, int value);
void shader_setInt(struct shader_t *shader, char *name, int value);
void shader_setFloat(struct shader_t *shader, char *name, float value);
unsigned shader_getUniformLoc(struct shader_t *shader, char *name);
void shader_setVec3(struct shader_t *shader, char *name, float *value);
void shader_setMat4(struct shader_t *shader, char *name, float *value);
void shader_setMaterial(struct shader_t *shader, struct material_t *material);
void shader_setLight(struct shader_t *shader, struct light_t *light);
#endif /* ifndef STRUCT_H */
