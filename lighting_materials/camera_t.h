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
    float zoom;
    vec3 pos;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 worldUp;
} Camera;

struct camera_t *new_Camera(vec3 pos, vec3 front, vec3 up, float speed);
void delete_Camera(struct camera_t *camera);
void camera_rotate(struct camera_t *camera, float yaw, float pitch);
void camera_getViewMatrix(struct camera_t *camera, mat4 view);
void camera_getProjectionMatrix(struct camera_t *camera, float angle, int width, int height, mat4 projection);

#endif
