#ifndef ENGINE_H
#define ENGINE_H

#include <android_native_app_glue.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <stdbool.h>
#include <math.h>

#define STATE_MENU    0
#define STATE_PLAYING 1

struct engine {
    struct android_app* app;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width, height;

    int gameState;
    float camPos[3];
    float camRot[2];
    float velY;
    bool onGround;

    float moveDirX, moveDirZ;
    int movePointerId;
    int lookPointerId;
    float lastTouchX, lastTouchY;

    GLuint program;
    GLuint uiProgram;
};
#endif
