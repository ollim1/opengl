// Compile with -DGL_GLEXT_PROTOTYPES
#include "include/cglm/cglm.h"
#include "include/cglm/call.h"
#include <SDL2/SDL.h>
#include <GL/gl.h>
// Compile with -DGL_GLEXT_PROTOTYPES
#include <GL/glext.h>
#define  STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"
#include "shader_t.h"
#include "shader_source.h"

float texCoords[] = {
    0.0f, 0.0f, // lower left
    1.0f, 0.0f, // lower right
    0.5f, 1.0f, // top center
};


int main(int argc, char **argv) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    int width, height;
    width = height = 0;
    if (argc == 3) {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
    }
    if (width == 0 || height == 0) {
        width = 800;
        height = 600;
    }

    SDL_Window *w = SDL_CreateWindow("test", 0, 0, width, height, SDL_WINDOW_OPENGL);
    SDL_GL_CreateContext(w);

    glClearColor(0.25, 0., 0., 1.);
    glEnable(GL_DEPTH_TEST);

#include "geometry.h"
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

    // setting up camera
    vec3 cameraPos = {0, 0, 3};
    vec3 cameraFront = {0, 0, -1};
    vec3 cameraUp = {0, 1, 0};

    SDL_Event e;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    while(1) {
        float currentFrame = SDL_GetTicks();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        int error = glGetError();
        if (error != GL_NO_ERROR) {
            printError(error);
        }
        if (processInput(e)) {
            // quit program if exit input detected
            break;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // view matrix
        float radius = 10;
        float camX = sin(SDL_GetTicks() / 1000.0f) * radius;
        float camZ = cos(SDL_GetTicks() / 1000.0f) * radius;
        mat4 view = {
            {1, 0, 0, 0,},
            {0, 1, 0, 0,},
            {0, 0, 1, 0,},
            {0, 0, 0, 1,},
        };
        glm_lookat(
                (vec3){camX, 0, camZ},
                (vec3){0, 0, 0},
                (vec3){0, 1, 0},
                view);

        // projection matrix
        mat4 projection;
        glm_perspective(glm_rad(45), (float) width / (float) height, 0.1f, 100.0f, projection);

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float *) view);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (float *) projection);

        // render container
        shader_use(shader);
        glBindVertexArray(VAO);
        for (int i = 0; i < 10; i++) {
            // model matrix
            mat4 model = {
                {1, 0, 0, 0,},
                {0, 1, 0, 0,},
                {0, 0, 1, 0,},
                {0, 0, 0, 1,},
            };
            glm_translate(model, cubePositions[i]);
            glm_rotate(model, glm_rad(-66), (vec3){1, 0, 0});
            glm_rotate(model, glm_rad(-66), cubePositions[i]);
	    float angle = 20 * i;
            glm_rotate(model, SDL_GetTicks() / 1000.0 + glm_rad(angle), (vec3){0, 0, 1});
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *) model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        SDL_GL_SwapWindow(w);
        SDL_PollEvent(&e);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    delete_Shader(shader);

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
    if (e.type == SDL_QUIT) {
        return 1;
    } else if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_q) {
            return 1;
        }
    }
    return 0;
}

