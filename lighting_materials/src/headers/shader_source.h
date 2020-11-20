#ifndef SHADER_SOURCE
#define SHADER_SOURCE value

const char *lightingVS =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"}\n";

const char *lightingFS =
"#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec3 objectColor;\n"
"uniform vec3 lightColor;\n"
"void main()\n"
"{\n"
"    float ambientStrength = 0.1;\n"
"    vec3 ambient = ambientStrength * lightColor;\n"
"    vec3 result = ambient * objectColor;\n"
"    FragColor = vec4(result, 1.0);\n"
"}\n";

const char *lightCubeFS =
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(1.0); // set all 4 vector values to 1.0\n"
"}\n";

#endif /* ifndef SHADER_SOURCE */
