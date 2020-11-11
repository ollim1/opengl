#ifndef CAMERA_H
#define CAMERA_H
#include "include/cglm/cglm.h"
#include "include/cglm/call.h"

typedef struct camera_t {
    float yaw;
    float pitch;
    float lastX;
    float lastY;
    float fov;
    vec3 pos;
    vec3 front;
    vec3 up;
} Camera;

struct camera_t *new_camera(vec3 pos, vec3 front, vec3 up);
void delete_camera(struct camera_t *camera);

#endif
