#include <SDL2/SDL.h>
#include <GL/gl.h>
// Compile with -DGL_GLEXT_PROTOTYPES
#include <GL/glext.h>
#include "include/cglm/cglm.h"
#include "include/cglm/call.h"

int main()
{
    vec4 v = {1.0f, 0.0f, 0.0f, 1.0f};
    mat4 m = {
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0,
    };

    glm_translate(m, (vec3){1.0, 1.0, 0.0});
}

void printError(int code)
{
    printf("gl error: ");
    switch (code) {
        case GL_INVALID_ENUM:
            puts("invalid  enum");
            break;
        case GL_INVALID_VALUE:
            puts("invalid value");
            break;
        case GL_INVALID_OPERATION:
            puts("invalid operation");
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            puts("invalid framebuffer operation");
            break;
        case GL_OUT_OF_MEMORY:
            puts("out of memory");
            break;
        case GL_STACK_UNDERFLOW:
            puts("stack underflow");
            break;
        case GL_STACK_OVERFLOW:
            puts("stack overflow");
            break;
        case 0:
            puts("glGetError error");
            break;
    }
}
