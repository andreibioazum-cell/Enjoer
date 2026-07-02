#include "engine.h"
#include "render.h"
#include "input.h"

static void init_egl(struct engine* eng) {
    eng->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(eng->display, 0, 0);
    const EGLint attr[] = { EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8, EGL_DEPTH_SIZE, 24, EGL_NONE };
    EGLConfig config; EGLint numConfig;
    eglChooseConfig(eng->display, attr, &config, 1, &numConfig);
    eng->surface = eglCreateWindowSurface(eng->display, config, eng->app->window, NULL);
    EGLint ctxAttr[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    eng->context = eglCreateContext(eng->display, config, NULL, ctxAttr);
    eglMakeCurrent(eng->display, eng->surface, eng->surface, eng->context);
    eglQuerySurface(eng->display, eng->surface, EGL_WIDTH, &eng->width);
    eglQuerySurface(eng->display, eng->surface, EGL_HEIGHT, &eng->height);
    init_graphics(eng);
}

static void term_egl(struct engine* eng) {
    eglMakeCurrent(eng->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(eng->display, eng->context);
    eglDestroySurface(eng->display, eng->surface);
    eglTerminate(eng->display);
}

static void on_cmd(struct android_app* app, int32_t cmd) {
    struct engine* eng = (struct engine*)app->userData;
    switch (cmd) {
        case APP_CMD_INIT_WINDOW: init_egl(eng); break;
        case APP_CMD_TERM_WINDOW: term_egl(eng); break;
    }
}

void android_main(struct android_app* state) {
    struct engine eng = {0};
    eng.app = state; eng.camPos[1] = 2.0f;
    state->userData = &eng; state->onAppCmd = on_cmd; state->onInputEvent = handle_input;

    while (1) {
        int events; struct android_poll_source* source;
        while (ALooper_pollAll(eng.gameState == STATE_PLAYING ? 0 : -1, NULL, &events, (void**)&source) >= 0) {
            if (source) source->process(state, source);
            if (state->destroyRequested) return;
        }

        if (eng.display && eng.gameState == STATE_PLAYING) {
            eng.velY -= 0.008f; eng.camPos[1] += eng.velY;
            if (eng.camPos[1] < 2.0f) { eng.camPos[1] = 2.0f; eng.velY = 0; eng.onGround = true; }
            float s = sinf(eng.camRot[1]), c = cosf(eng.camRot[1]);
            eng.camPos[0] += (c * eng.moveDirX + s * eng.moveDirZ) * 0.1f;
            eng.camPos[2] += (s * eng.moveDirX - c * eng.moveDirZ) * 0.1f;

            glViewport(0, 0, eng.width, eng.height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glUseProgram(eng.program);
            float p[16], v[16], m[16];
            mat4_perspective(p, 1.2f, (float)eng.width/eng.height, 0.1f, 100.0f);
            mat4_lookat(v, eng.camPos, eng.camRot[0], eng.camRot[1]);
            mat4_mul(m, p, v);
            glUniformMatrix4fv(glGetUniformLocation(eng.program, "m"), 1, GL_FALSE, m);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 20, cube_vertices);
            glEnableVertexAttribArray(0);
            glDrawArrays(GL_TRIANGLES, 0, 36); // Рисуем один куб как платформу
            
            glDisable(GL_DEPTH_TEST);
            draw_rect(&eng, 200, eng.height-200, 50, 50, 0,0,0); // Джойстик
            draw_rect(&eng, eng.width-150, eng.height-150, 40, 40, 1,0,0); // Прыжок
            eglSwapBuffers(eng.display, eng.surface);
        } else if (eng.display && eng.gameState == STATE_MENU) {
            glClear(GL_COLOR_BUFFER_BIT);
            draw_rect(&eng, eng.width/2, eng.height/2, 100, 50, 0, 0.8f, 0); // Кнопка Play
            eglSwapBuffers(eng.display, eng.surface);
        }
    }
}
