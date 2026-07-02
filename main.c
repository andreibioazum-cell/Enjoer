#include "engine.h"
#include "render.h"
#include "input.h"
#include "math_utils.h"
#include "cube.h"

static void init_egl(struct engine* eng) {
    eng->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(eng->display, 0, 0);
    EGLConfig config; EGLint num;
    EGLint attr[] = { EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8, EGL_DEPTH_SIZE, 24, EGL_NONE };
    eglChooseConfig(eng->display, attr, &config, 1, &num);
    eng->surface = eglCreateWindowSurface(eng->display, config, eng->app->window, NULL);
    EGLint cattr[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    eng->context = eglCreateContext(eng->display, config, NULL, cattr);
    eglMakeCurrent(eng->display, eng->surface, eng->surface, eng->context);
    eglQuerySurface(eng->display, eng->surface, EGL_WIDTH, &eng->width);
    eglQuerySurface(eng->display, eng->surface, EGL_HEIGHT, &eng->height);
    init_render(eng);
}

static void on_cmd(struct android_app* app, int32_t cmd) {
    if (cmd == APP_CMD_INIT_WINDOW) init_egl((struct engine*)app->userData);
}

void android_main(struct android_app* state) {
    struct engine eng = {0};
    eng.app = state; eng.camPos[1] = 1.7f; eng.movePointerId = -1; eng.lookPointerId = -1;
    state->userData = &eng; state->onAppCmd = on_cmd; state->onInputEvent = handle_input;

    while (1) {
        int evs; struct android_poll_source* src;
        while (ALooper_pollAll(eng.gameState == STATE_PLAYING ? 0 : -1, NULL, &evs, (void**)&src) >= 0) {
            if (src) src->process(state, src);
            if (state->destroyRequested) return;
        }

        if (eng.display) {
            glClearColor(0.2f, 0.2f, 0.25f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (eng.gameState == STATE_PLAYING) {
                // Физика (Roblox-style)
                eng.velY -= 0.008; eng.camPos[1] += eng.velY;
                if (eng.camPos[1] < 1.7) { eng.camPos[1] = 1.7; eng.velY = 0; eng.onGround = true; }
                float s = sinf(eng.camRot[1]), c = cosf(eng.camRot[1]);
                eng.camPos[0] += (c * eng.moveDirX + s * eng.moveDirZ) * 0.12;
                eng.camPos[2] += (s * eng.moveDirX - c * eng.moveDirZ) * 0.12;

                // Рендер мира
                glEnable(GL_DEPTH_TEST);
                glUseProgram(eng.program);
                float p[16], v[16], pv[16], m[16], mvp[16];
                mat4_perspective(p, 1.1f, (float)eng.width/eng.height, 0.1f, 100.0f);
                mat4_lookat(v, eng.camPos, eng.camRot[0], eng.camRot[1]);
                mat4_mul(pv, p, v);

                // Рисуем платформу 10x10
                for(int x=-5; x<=5; x++) for(int z=-5; z<=5; z++) {
                    mat4_identity(m); m[12]=x; m[13]=0; m[14]=z;
                    mat4_mul(mvp, pv, m);
                    glUniformMatrix4fv(glGetUniformLocation(eng.program, "m"), 1, GL_FALSE, mvp);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 24, cube_vertices);
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 24, cube_vertices+3);
                    glEnableVertexAttribArray(1);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
                glDisable(GL_DEPTH_TEST);
                draw_ui_box(&eng, 200, eng.height-200, 70, 70, 1,1,1, 0.3); // Стик
                draw_ui_box(&eng, eng.width-150, eng.height-150, 50, 50, 1,0,0, 0.5); // Прыжок
            } else {
                draw_ui_box(&eng, eng.width/2, eng.height/2, 150, 60, 0.1, 0.8, 0.1, 1.0); // Кнопка Play
            }
            eglSwapBuffers(eng.display, eng.surface);
        }
    }
}
