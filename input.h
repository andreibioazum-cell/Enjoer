#ifndef INPUT_H
#define INPUT_H
#include "engine.h"

static int32_t handle_input(struct android_app* app, AInputEvent* ev) {
    struct engine* eng = (struct engine*)app->userData;
    if (AInputEvent_getType(ev) != AINPUT_EVENT_TYPE_MOTION) return 0;
    int a = AMotionEvent_getAction(ev), type = a & AMOTION_EVENT_ACTION_MASK;
    int idx = (a & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
    float x = AMotionEvent_getX(ev, idx), y = AMotionEvent_getY(ev, idx);
    int id = AMotionEvent_getPointerId(ev, idx);

    if (type == AMOTION_EVENT_ACTION_DOWN || type == AMOTION_EVENT_ACTION_POINTER_DOWN) {
        if (eng->gameState == STATE_MENU) {
            if (fabs(x - eng->width/2) < 200 && fabs(y - eng->height/2) < 100) eng->gameState = STATE_PLAYING;
        } else {
            if (x < eng->width/2) eng->movePointerId = id;
            else if (x > eng->width*0.75 && y > eng->height*0.75) { if(eng->onGround){eng->velY=0.15; eng->onGround=false;} }
            else { eng->lookPointerId = id; eng->lastTouchX = x; eng->lastTouchY = y; }
        }
    } else if (type == AMOTION_EVENT_ACTION_UP || type == AMOTION_EVENT_ACTION_POINTER_UP) {
        if (id == eng->movePointerId) { eng->movePointerId = -1; eng->moveDirX = eng->moveDirZ = 0; }
        if (id == eng->lookPointerId) eng->lookPointerId = -1;
    } else if (type == AMOTION_EVENT_ACTION_MOVE) {
        for(int i=0; i<AMotionEvent_getPointerCount(ev); i++) {
            int pid = AMotionEvent_getPointerId(ev, i);
            float px = AMotionEvent_getX(ev, i), py = AMotionEvent_getY(ev, i);
            if (pid == eng->movePointerId) { eng->moveDirX = (px - 200)/100.0; eng->moveDirZ = (py - (eng->height-200))/100.0; }
            else if (pid == eng->lookPointerId) { eng->camRot[1] += (px - eng->lastTouchX)*0.005; eng->camRot[0] += (py - eng->lastTouchY)*0.005; eng->lastTouchX = px; eng->lastTouchY = py; }
        }
    }
    return 1;
}
#endif
