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
} Camera;

struct camera_t new_camera();
void delete_camera();

#endif
