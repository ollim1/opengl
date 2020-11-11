#include    "camera_t.h"

struct camera_t *new_camera(vec3 pos, vec3 front, vec3 up)
{
    struct camera_t *camera = malloc(sizeof(struct camera_t));
    if (!camera) {
        puts("failed to alloc camera");
        return NULL;
    }

    memcpy(camera->pos, pos, sizeof(vec3));
    memcpy(camera->front, front, sizeof(vec3));
    memcpy(camera->up, up, sizeof(vec3));
    return camera;
}
void delete_camera(struct camera_t *camera)
{
    if (camera)
        free(camera);
}
