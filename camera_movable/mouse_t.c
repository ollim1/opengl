#include    "mouse_t.h"
#include    <stdlib.h>

struct mouse_t *new_mouse(float sensitivity, int startX, int startY)
{
    struct mouse_t *mouse = malloc(sizeof(struct mouse_t));
    if (!mouse)
        return NULL;

    mouse->lastX = startX;
    mouse->lastY = startY;
    mouse->first = 0;
    mouse->sensitivity = sensitivity;
    return mouse;
}

void delete_mouse(struct mouse_t *mouse)
{
    if (mouse)
        free(mouse);
}
