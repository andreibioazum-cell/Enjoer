#ifndef INPUT_H
#define INPUT_H
#include "engine.h"

static int32_t handle_input(struct android_app* app, AInputEvent* event) {
    struct engine* eng = (struct engine*)app->userData;
    if (AInputEvent_getType(event) != AINPUT_EVENT_TYPE_MOTION) return 0;
    int action = AMotionEvent_getAction(event);
    int type = action & AMOTION_EVENT_ACTION_MASK;
    int idx = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
    float x = AMotionEvent_getX(event, idx), y = AMotionEvent_getY(event, idx);
    int id = AMotionEvent_getPointerId(event, idx);

    if (type == AMOTION_EVENT_ACTION_DOWN || type == AMOTION_EVENT_ACTION_POINTER_DOWN) {
        if (eng->gameState == STATE_MENU) {
            if (fabs(x - eng->width/2) < 200 && fabs(y - eng->height/2) < 100) eng->gameState = STATE_PLAYING;
        } else {
            if (x < eng->width/2) eng->movePointerId = id;
            else if (x > eng->width - 250 && y > eng->height - 250) { 
                if(eng->onGround) { eng->velY = 0.15f; eng->onGround = false; }
            } else { eng->lookPointerId = id; eng->lastTouchX = x; eng->lastTouchY = y; }
        }
    } else if (type == AMOTION_EVENT_ACTION_MOVE) {
        for (int i=0; i<AMotionEvent_getPointerCount(event); i++) {
            if (AMotionEvent_getPointerId(event, i) == eng->movePointerId) {
                eng->moveDirX = (AMotionEvent_getX(event, i) - 200)/100.0f;
                eng->moveDirZ = (AMotionEvent_getY(event, i) - (eng->height-200))/100.0f;
            } else if (AMotionEvent_getPointerId(event, i) == eng->lookPointerId) {
                float curX = AMotionEvent_getX(event, i), curY = AMotionEvent_getY(event, i);
                eng->camRot[1] += (curX - eng->lastTouchX) * 0.005f;
                eng->camRot[0] += (curY - eng->lastTouchY) * 0.005f;
                eng->lastTouchX = curX; eng->lastTouchY = curY;
            }
        }
    } else if (type == AMOTION_EVENT_ACTION_UP || type == AMOTION_EVENT_ACTION_POINTER_UP) {
        if (id == eng->movePointerId) { eng->movePointerId = -1; eng->moveDirX = 0; eng->moveDirZ = 0; }
        if (id == eng->lookPointerId) eng->lookPointerId = -1;
    }
    return 1;
}
#endif
