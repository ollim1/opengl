// Compile with -DGL_GLEXT_PROTOTYPES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/cglm/cglm.h"
#include "include/cglm/call.h"
#include <SDL2/SDL.h>
#include <GL/gl.h>
// Compile with -DGL_GLEXT_PROTOTYPES
#include <GL/glext.h>
#define  STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"
#include "libs/camera_t.h"
#include "libs/mouse_t.h"
#include "libs/shader_t.h"
#include "libs/material_t.h"
#include "libs/light_t.h"
#include "libs/strbuild_t.h"
#include "libs/fileload.h"
#include <math.h>

float deltaTime;
Camera *camera;
Mouse *mouse;
int KEYS[322]; // SDL keysyms

int processInput(SDL_Event e);
void printError(int code);
int loadTexture(char *name, unsigned slot);
void handleInput();
void processMouseInput(float xpos, float ypos);
void setupWindow();
void moveObject(vec3 loc, float delta);
char *getPath(char *arg);
char *getFullPath(char *rel, char *exec);

int main(int argc, char **argv) {
    // get executable path for access to file resources with relative paths
    char *pwd = getPath(argv[0]);
    if (!pwd) {
        puts("couldn't get path");
        return -1;
    }

    vec3 lightPos = {1.2f, 1.0f, 2.0f};

    char *lightingVPath = getFullPath("shaders/lightingV.glsl", argv[0]);
    printf("%s\n", lightingVPath);
    char *lightingVS = loadToStr(lightingVPath);
    free(lightingVPath);

    char *lightingFPath = getFullPath("shaders/lightingF.glsl", argv[0]);
    printf("%s\n", lightingFPath);
    char *lightingFS = loadToStr(lightingFPath);
    free(lightingFPath);

    char *lightCubeFPath = getFullPath("shaders/lightCubeF.glsl", argv[0]);
    printf("%s\n", lightCubeFPath);
    char *lightCubeFS = loadToStr(lightCubeFPath);
    free(lightCubeFPath);

    if (!lightingVS || !lightingFS || !lightCubeFS) {
        puts("failed to read shader files");
        return -1;
    }
    setupWindow();

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

#include "headers/geometry.h"

    vec3 cubePositions[] = {
         { 0.0f,  0.0f,  0.0f},
        { 2.0f,  5.0f, -15.0f},
        {-1.5f, -2.2f, -2.5f},
        {-3.8f, -2.0f, -12.3f},
        { 2.4f, -0.4f, -3.5f},
        {-1.7f,  3.0f, -7.5f},
        { 1.3f, -2.0f, -2.5f},
        { 1.5f,  2.0f, -2.5f},
        { 1.5f,  0.2f, -1.5f},
        {-1.3f,  1.0f, -1.5f},
    };
        
    // set up buffers to configure vertex attributes
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // light VAO
    unsigned lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

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

    // load texture
    unsigned diffuse_map, specular_map;
    if (!(diffuse_map = loadTexture("textures/container2.png", GL_TEXTURE0))
            || !(specular_map = loadTexture("textures/container2_specular.png", GL_TEXTURE1))) {
        puts("failed to load textures");
        return -1;
    }
    // creating material struct
    Material *material;
    if (!(material = new_Material(0, 1, 32.0f))) {
        puts("failed to create material struct");
        return -1;
    }
        
    // creating light struct
    Light *light;
    if (!(light = new_Light(
                    (vec4){1.2f, 1.0f, 2.0f, 1.0f},
                    (vec3){0.2f, 0.2f, 0.2f},
                    (vec3){0.5f, 0.5f, 0.5f},
                    (vec3){1.0f, 1.0f, 1.0f},
                    1.0f,
                    0.09f,
                    0.032f,
                    cos(glm_rad(12.5f))))) {
        puts("failed to create light struct");
        return -1;
    }

    // setting shader uniform parameters
    shader_use(lightingShader);
    shader_setMaterial(lightingShader, material);
    shader_setLight(lightingShader, light);
    shader_setVec3(lightingShader, "viewPos", camera->pos);

    float lastFrame = 0.0f;
    SDL_Event e;
    int loopShouldTerminate = 0;
    while(1) {
        float currentFrame = SDL_GetTicks();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

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

//        moveObject(light->position, deltaTime);
        shader_use(lightingShader);
        moveObject(light->direction, deltaTime);
        shader_setLight(lightingShader, light);
        shader_setVec3(lightingShader, "viewPos", camera->pos);

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
        shader_use(lightingShader);
        shader_setMat4(lightingShader, "view", (float *) view);
        shader_setMat4(lightingShader, "projection", (float *) projection);
        // bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse_map);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specular_map);

        mat4 model;
        for (int i = 0; i < 10; i++) {
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            memcpy(model, (mat4){
                    {1, 0, 0, 0,},
                    {0, 1, 0, 0,},
                    {0, 0, 1, 0,},
                    {0, 0, 0, 1,},
                    }, sizeof(mat4));
            glm_translate(model, cubePositions[i]);
            glm_rotate(model, glm_rad(-66), (vec3){1, 0, 0});
            glm_rotate(model, glm_rad(-66), cubePositions[i]);
            shader_setMat4(lightingShader, "model", (float *) model);
        }

        // light cube
        shader_use(lightCubeShader);
        shader_setMat4(lightCubeShader, "view", (float *) view);
        shader_setMat4(lightCubeShader, "projection", (float *) projection);
        memcpy(model, (mat4){
            {1, 0, 0, 0,},
            {0, 1, 0, 0,},
            {0, 0, 1, 0,},
            {0, 0, 0, 1,},
        }, sizeof(mat4));
        glm_translate(model, light->direction);
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
    delete_Material(material);
    delete_Light(light);
    free(lightingVS);
    free(lightingFS);
    free(lightCubeFS);
    free(pwd);

    return 0;
}

int loadTexture(char *name, unsigned slot)
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
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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

void moveObject(vec3 loc, float delta)
{
    if (!loc)
        return;

    mat4 m = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
    };
    vec3 pivot = {0, 0, 0};
    vec3 axis = {0, 1, 0};
    glm_rotate_at(m, pivot, glm_rad(delta / 25), axis);
    glm_mat4_mulv3(m, loc, 1, loc);
//    printf("%5p: %f %f %f\n", loc, loc[0], loc[1], loc[2]);
}

char *getFullPath(char *rel, char *exec)
{
    // turn relative path of resource into full path
    char *basePath = getPath(exec);
    StringBuilder *sb = new_StringBuilder(basePath);
    stringBuilder_append(sb, rel);
    char *fullPath = stringBuilder_getStr(sb);
    printf("%s\n", fullPath);
    delete_StringBuilder(sb);
    return fullPath;
}

char *getPath(char *arg)
{
    // get path of executable
    size_t len = strlen(arg);
    char *str = calloc((len + 1), sizeof(char));
    if (!str) {
        puts("malloc failed");
        return NULL;
    }
    strncpy(str, arg, len);

    for (int i = len - 1; i >= 0 && str[i] != '/'; i--) {
        str[i] = '\0';
    }
    return str;
}
