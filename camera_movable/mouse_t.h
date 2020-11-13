#ifndef MOUSE_T_H
#define MOUSE_T_H

typedef struct mouse_t {
    int lastX;
    int lastY;
    int first;
    float sensitivity;
} Mouse;

struct mouse_t *new_mouse(float sensitivity, int startX, int startY);
void delete_mouse(struct mouse_t *mouse);

#endif /* ifndef MOUSE_T_H */

