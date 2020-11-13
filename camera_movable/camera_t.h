#ifndef CAMERA_T_H
#define CAMERA_T_H
#include "include/cglm/cglm.h"
#include "include/cglm/call.h"

typedef struct camera_t {
    float yaw;
    float pitch;
    float lastX;
    float lastY;
    float fov;
    float speed;
    vec3 pos;
    vec3 front;
    vec3 up;
} Camera;

struct camera_t *new_camera(vec3 pos, vec3 front, vec3 up, float speed);
void delete_camera(struct camera_t *camera);

#endif
