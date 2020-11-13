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

    camera = new_Camera((vec3){0.0f, 0.0f, 3.0f}, (vec3){0.0f, 0.0f, -1.0f}, (vec3){0.0f, 1.0f, 0.0f}, 3.0f);
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

    int randCubeCount = 10000;
    int randCubeRange = 50;
    vec3 *randCubePos = malloc(randCubeCount * sizeof(vec3));;
    if (!randCubePos) {
        puts("failed to allocate space for cube position array");
        return -1;
    }
    for (int i = 0; i < randCubeCount; ++i) {
        memcpy(randCubePos[i], (vec3){
                (random() % randCubeRange) - randCubeRange / 2,
                (random() % randCubeRange) - randCubeRange / 2,
                (random() % randCubeRange) - randCubeRange / 2, }, sizeof(vec3));
    }
        
    // set up buffers to configure vertex attributes
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // texture coordinate
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // compiling shaders
    Shader *shader = new_Shader(vertexShaderSource, fragShaderSource);
    if (!shader) {
        puts("failed to load shaders");
        return -1;
    }

    unsigned texture1, texture2;
    if (!(texture1 = load_texture("wall.jpg", GL_RGB, GL_TEXTURE0))
            || !(texture2 = load_texture("awesomeface.png", GL_RGBA, GL_TEXTURE1)))
        return -1;

    // loading a picture onto the texture
    // setting samplers for shader
    shader_use(shader);
    shader_setInt(shader, "texture1", 0);
    shader_setInt(shader, "texture2", 1);
    unsigned int modelLoc = shader_getUniformLoc(shader, "model");
    unsigned int viewLoc = shader_getUniformLoc(shader, "view");
    unsigned int projectionLoc = shader_getUniformLoc(shader, "projection");

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

        // bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // view matrix
        mat4 view = {
            {1, 0, 0, 0,},
            {0, 1, 0, 0,},
            {0, 0, 1, 0,},
            {0, 0, 0, 1,},
        };
        vec3 front;
        glm_vec3_add(camera->pos, camera->front, front);
        glm_lookat(
                camera->pos,
                front,
                camera->up,
                view);

        // projection matrix
        mat4 projection;
        glm_perspective(glm_rad(45), (float) width / (float) height, 0.1f, 100.0f, projection);

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float *) view);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (float *) projection);

        // render container
        shader_use(shader);
        glBindVertexArray(VAO);
        for (int i = 0; i < randCubeCount; i++) {
            // model matrix
            mat4 model = {
                {1, 0, 0, 0,},
                {0, 1, 0, 0,},
                {0, 0, 1, 0,},
                {0, 0, 0, 1,},
            };
            glm_translate(model, randCubePos[i]);
            glm_rotate(model, glm_rad(-66), (vec3){1, 0, 0});
            glm_rotate(
                    model,
                    glm_rad((int) (
                            (randCubePos[i][1] * 31)
                            + (randCubePos[i][1] * 31)
                            + (randCubePos[i][1] * 31)) % 180),
                    randCubePos[i]);
	    float angle = 20 * i;
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *) model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        SDL_GL_SwapWindow(w);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    delete_Shader(shader);
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
