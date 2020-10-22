#include <SDL2/SDL.h>
#include <GL/gl.h>
// Compile with -DGL_GLEXT_PROTOTYPES
#include <GL/glext.h>

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\n\0";


const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(mod(gl_FragCoord.y / 10, 2.), 0.7, 1.0, 1.0);\n"
    "}\n\0";

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *w = SDL_CreateWindow("test", 0, 0, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GL_CreateContext(w);

    glClearColor(0.25, 0., 0., 1.);

    // Compiling or linking this should fail if things aren't set up correctly
    // creating a shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("shader compilation error: %s\n", infoLog);
        return -1;
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("shader compilation error: %s\n", infoLog);
        return -1;
    }

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("shader linking error: %s\n", infoLog);
        return -1;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // triangle creation start
    float vertices[] = {
        -0.5f,  0.5f, 0.0f,
         0.0f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f
    };

    // generate VAO
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    SDL_Event e;
    while(1) {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(w);

        SDL_PollEvent(&e);
        int error = glGetError();
        if (error != GL_NO_ERROR) {
            printf("gl error: ");
            switch (error) {
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
        if (e.type == SDL_QUIT) {
            break;
        } else if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_q) {
                break;
            }
        }
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    return 0;
}
