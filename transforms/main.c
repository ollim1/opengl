// Compile with -DGL_GLEXT_PROTOTYPES
#include "include/cglm/cglm.h"
#include "include/cglm/call.h"

int main()
{
    mat4 m = {
        {1, 0, 0, 0,},
        {0, 1, 0, 0,},
        {0, 0, 1, 0,},
        {0, 0, 0, 1,},
    };

    glm_rotate(m, glm_rad(90), (vec3){0, 0, 1});
    glm_scale(m, (vec3){0.5, 0.5, 0.5});
    vec3 v = {1, 0, 0};
    glm_mat4_mulv3(m, v, 1, v);
    printf("%f %f %f\n", v[0], v[1], v[2]);
}

