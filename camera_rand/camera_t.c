#include    "camera_t.h"
#include    <string.h>

void camera_updateVectors(struct camera_t *camera);
struct camera_t *new_Camera(vec3 pos, vec3 front, vec3 up, float speed)
{
    struct camera_t *camera = malloc(sizeof(struct camera_t));
    if (!camera)
        return NULL;

    // init yaw to -90.0 since 0.0 results in a rightward direction vector
    camera->yaw = -90.f;
    camera->pitch = 0.0f;
    memcpy(camera->pos, pos, sizeof(vec3));
    memcpy(camera->front, front, sizeof(vec3));
    memcpy(camera->worldUp, up, sizeof(vec3));
    camera->speed = speed;
    camera_updateVectors(camera);

    return camera;
}
void delete_Camera(struct camera_t *camera)
{
    if (camera)
        free(camera);
}
void camera_rotate(struct camera_t *camera, float yaw, float pitch)
{
    camera->yaw += yaw;
    camera->pitch += pitch;

    // oob check
    if (camera->pitch > 89.9f)
        camera->pitch  = 89.9f;
    if (camera->pitch < -89.9f)
        camera->pitch  = -89.9f;

    camera_updateVectors(camera);
}
void camera_updateVectors(struct camera_t *camera)
{
    // front vector
    vec3 front = {
        cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch)),
        sin(glm_rad(camera->pitch)),
        sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch)),
    };
    glm_normalize(front);
    memcpy(camera->front, front, sizeof(vec3));
    // right vector
    glm_vec3_crossn(camera->front, camera->worldUp, camera->right);
    // up vector
    glm_vec3_crossn(camera->right, camera->front, camera->up);
}
