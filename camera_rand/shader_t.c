#include <SDL2/SDL.h>
#include <GL/gl.h>
// Compile with -DGL_GLEXT_PROTOTYPES
#include <GL/glext.h>
#include "shader_t.h"

unsigned compileShader(char *shaderSource, unsigned shaderType);

struct shader_t *new_Shader(char *vertexShaderSource, char *fragShaderSource)
{
    unsigned vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
    unsigned fragShader = compileShader(fragShaderSource, GL_FRAGMENT_SHADER);
    if (vertexShader == 0 || fragShader == 0)
        return NULL;

    // linking shaders
    int success;
    char infoLog[512];
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("shader linking error: %s\n", infoLog);
        return NULL;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);


    Shader *ret = malloc(sizeof(Shader));
    if (ret)
        ret->id = shaderProgram;
    return ret;
}

unsigned compileShader(char *shaderSource, unsigned shaderType)
{
    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("shader compilation error: %s\n", infoLog);
        return 0;
    }
    return shader;
}

void delete_Shader(struct shader_t *shader)
{
    if (shader) {
        glDeleteProgram(shader->id);
        free(shader);
    }
}

void shader_use(struct shader_t *shader)
{
    if (!shader) {
        puts("error: no shader passed to shader_use");
        return;
    }
    glUseProgram(shader->id);
}

void shader_setBool(struct shader_t *shader, char *name, int value)
{
    if (!shader || !name)
        return;
    glUniform1i(glGetUniformLocation(shader->id, name), value);
}

void shader_setInt(struct shader_t *shader, char *name, int value)
{
    if (!shader || !name)
        return;
    glUniform1i(glGetUniformLocation(shader->id, name), value);
}

void shader_setFloat(struct shader_t *shader, char *name, float value)
{
    if (!shader || !name)
        return;
    glUniform1f(glGetUniformLocation(shader->id, name), value);
}

unsigned shader_getUniformLoc(struct shader_t *shader, char *name)
{
    if (!shader || !name)
        return 0;
    return glGetUniformLocation(shader->id, name);
}
void shader_setMatrix4(struct shader_t *shader, char *name, float *matrix)
{
    if (!shader || !name)
        return;
    glUniformMatrix4fv(glGetUniformLocation(shader->id, name), 1, GL_FALSE, matrix);
}
