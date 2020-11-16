// Compile with -DGL_GLEXT_PROTOTYPES
#include <stdio.h>
#include <stdlib.h>
#include "include/cglm/cglm.h"
#include "include/cglm/call.h"
#include <SDL2/SDL.h>
#include <GL/gl.h>
// Compile with -DGL_GLEXT_PROTOTYPES
#include <GL/glext.h>
#include <time.h>
#define  STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"
#include "camera_t.h"
#include "mouse_t.h"
#include "shader_t.h"
#include "shader_source.h"

float deltaTime;
Camera *camera;
Mouse *mouse;
int KEYS[322]; // SDL keysyms

int processInput(SDL_Event e);
void printError(int code);
int load_texture(char *name, unsigned mode, unsigned slot);
void handleInput();
void processMouseInput(float xpos, float ypos);
void setupWindow();

int main(int argc, char **argv) {
    setupWindow();
    srandom(time(NULL));

    camera = new_Camera(
            (vec3){0.0f, 0.0f, 3.0f},
            (vec3){0.0f, 0.0f, -1.0f},
            (vec3){0.0f, 1.0f, 0.0f},
            3.0f);
    if (!camera) {
        puts("failed to allocate memory for camera");
        return -1;
    }

    memset(KEYS, 0, 322 * sizeof(int));
    deltaTime = 0.0f;

    int width, height;
    width = height = 0;
    if (argc >= 3) {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
    }
    if (width == 0 || height == 0) {
        width = 800;
        height = 600;
    }
    float sensitivity = 0.0f;
    if (argc == 4) {
        sensitivity = atof(argv[3]);
    }
    if (sensitivity == 0.0f) {
        sensitivity = 0.1f;
    }

    if (!(mouse = new_Mouse(sensitivity, width / 2, height / 2))) {
        puts("failed to allocate memory for mouse struct");
        return -1;
    }

    SDL_Window *w = SDL_CreateWindow("test", 0, 0, width, height, SDL_WINDOW_OPENGL);
    SDL_GL_CreateContext(w);

    glClearColor(0.25, 0., 0., 1.);
    glEnable(GL_DEPTH_TEST);

#include "geometry.h"
        
    // set up buffers to configure vertex attributes
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // light VAO
    unsigned lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    vec3 lightPos = {1.2f, 1.0f, 2.0f};

    // compiling shaders
    Shader *lightingShader = new_Shader(lightingVS, lightingFS);
    if (!lightingShader) {
        puts("failed to load shaders");
        return -1;
    }
    Shader *lightCubeShader = new_Shader(lightingVS, lightCubeFS);
    if (!lightCubeShader) {
        puts("failed to load shaders");
        return -1;
    }

    // loading a picture onto the texture
    // setting samplers for shader
    shader_use(lightingShader);
    shader_setVec3(lightingShader, "objectColor", (vec3){1.0f, 0.5f, 0.31f});
    shader_setVec3(lightingShader, "lightColor", (vec3){1.0f, 1.0f, 1.0f});

    float lastFrame = 0.0f;
    SDL_Event e;
    int loopShouldTerminate = 0;
    while(1) {
        float currentFrame = SDL_GetTicks();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        int error = glGetError();
        if (error != GL_NO_ERROR) {
            printError(error);
        }

        while (SDL_PollEvent(&e)) {
            if (processInput(e)) {
                // quit program if exit input detected
                loopShouldTerminate = 1;
                break;
            }
        }
        if (loopShouldTerminate)
            break;
        handleInput();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // view matrix
        mat4 view = {
            {1, 0, 0, 0,},
            {0, 1, 0, 0,},
            {0, 0, 1, 0,},
            {0, 0, 0, 1,},
        };
        camera_getViewMatrix(camera, view);

        // projection matrix
        mat4 projection;
        camera_getProjectionMatrix(camera, 45, width, height, projection);

        // world transformation
        mat4 model = {
            {1, 0, 0, 0,},
            {0, 1, 0, 0,},
            {0, 0, 1, 0,},
            {0, 0, 0, 1,},
        };

        shader_use(lightingShader);
        shader_setMat4(lightingShader, "view", (float *) view);
        shader_setMat4(lightingShader, "projection", (float *) projection);
        shader_setMat4(lightingShader, "model", (float *) model);

        // render first cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // second cube
        shader_use(lightCubeShader);
        shader_setMat4(lightCubeShader, "view", (float *) view);
        shader_setMat4(lightCubeShader, "projection", (float *) projection);
        memcpy(model, (mat4){
            {1, 0, 0, 0,},
            {0, 1, 0, 0,},
            {0, 0, 1, 0,},
            {0, 0, 0, 1,},
        }, sizeof(mat4));
        glm_translate(model, lightPos);
        glm_scale(model, (vec3){0.2f, 0.2f, 0.2f});
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        shader_setMat4(lightCubeShader, "model", (float *) model);

        SDL_GL_SwapWindow(w);
    }
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);
    delete_Shader(lightingShader);
    delete_Shader(lightCubeShader);
    delete_Camera(camera);
    delete_Mouse(mouse);

    return 0;
}

int load_texture(char *name, unsigned mode, unsigned slot)
{
    unsigned texture;
    // allocate textures
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // texture wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glActiveTexture(slot);
    glBindTexture(GL_TEXTURE_2D, texture);
    int width, height, nrChannels;
    unsigned char *data = stbi_load(name, &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, mode, width, height, 0, mode, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        puts("failed to load texture file");
        return 0;
    }
    stbi_image_free(data);
    return texture;
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

int processInput(SDL_Event e)
{
    // break loop if exit detected
    switch (e.type) {
        case SDL_QUIT:
            return 1;
            break;
        case SDL_KEYDOWN:
            if (e.key.keysym.sym < 322) {
                KEYS[e.key.keysym.sym] = 1;
                if (e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_q) {
                    return 1;
                }
            }
            break;
        case SDL_MOUSEMOTION:
            processMouseInput(e.motion.xrel, e.motion.yrel);
            break;
        case SDL_KEYUP:
            if (e.key.keysym.sym <= 321) {
                KEYS[e.key.keysym.sym] = 0;
            }
    }

    return 0;
}

void handleInput()
{
    // translation
    float fps = 1000.0f / deltaTime;
//    printf("fps: %10f\n", fps);
    if (KEYS[SDLK_w]) {
        vec3 temp;
        glm_vec3_scale(camera->front, camera->speed / fps, temp);
        glm_vec3_add(camera->pos, temp, camera->pos);
    }
    if (KEYS[SDLK_s]) {
        vec3 temp;
        glm_vec3_scale(camera->front, camera->speed / fps, temp);
        glm_vec3_sub(camera->pos, temp, camera->pos);
    }
    if (KEYS[SDLK_a]) {
        vec3 temp;
        glm_vec3_scale(camera->right, camera->speed / fps, temp);
        glm_vec3_sub(camera->pos, temp, camera->pos);
    }
    if (KEYS[SDLK_d]) {
        vec3 temp;
        glm_vec3_scale(camera->right, camera->speed / fps, temp);
        glm_vec3_add(camera->pos, temp, camera->pos);
    }
    if (KEYS[SDLK_r]) {
        vec3 temp;
        glm_vec3_scale(camera->up, camera->speed / fps, temp);
        glm_vec3_add(camera->pos, temp, camera->pos);
    }
    if (KEYS[SDLK_f]) {
        vec3 temp;
        glm_vec3_scale(camera->up, camera->speed / fps, temp);
        glm_vec3_sub(camera->pos, temp, camera->pos);
    }
}

void processMouseInput(float xpos, float ypos)
{
    if (mouse->first) {
        mouse->lastX = xpos;
        mouse->lastY = ypos;
        mouse->first = 0;
    }

    float xoffset = xpos - camera->lastX;
    float yoffset = ypos - camera->lastY;
    mouse->lastX = xpos;
    mouse->lastY = ypos;
    xoffset *= mouse->sensitivity;
    yoffset *= mouse->sensitivity;

    // for uninverted vertical axis
    yoffset *= -1;

    camera_rotate(camera, xoffset, yoffset);
}

void setupWindow()
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
}
