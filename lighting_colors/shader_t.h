#ifndef STRUCT_H
#define STRUCT_H value
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
#endif /* ifndef STRUCT_H */
