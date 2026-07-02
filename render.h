#ifndef RENDER_H
#define RENDER_H
#include "engine.h"
#include "math_utils.h"
#include "cube.h"

static GLuint compile_shader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);
    return s;
}

static void init_graphics(struct engine* eng) {
    const char* vWorld = "attribute vec4 p; attribute vec2 uv; variation vec2 vuv; uniform mat4 m; void main(){ gl_Position=m*p; vuv=uv; }";
    const char* fWorld = "precision mediump float; variation vec2 vuv; void main(){ gl_FragColor=vec4(vuv, 1.0, 1.0); }";
    eng->program = glCreateProgram();
    glAttachShader(eng->program, compile_shader(GL_VERTEX_SHADER, vWorld));
    glAttachShader(eng->program, compile_shader(GL_FRAGMENT_SHADER, fWorld));
    glLinkProgram(eng->program);

    const char* vUI = "attribute vec4 p; uniform vec4 col; void main(){ gl_Position=p; }";
    const char* fUI = "precision mediump float; uniform vec4 col; void main(){ gl_FragColor=col; }";
    eng->uiProgram = glCreateProgram();
    glAttachShader(eng->uiProgram, compile_shader(GL_VERTEX_SHADER, vUI));
    glAttachShader(eng->uiProgram, compile_shader(GL_FRAGMENT_SHADER, fUI));
    glLinkProgram(eng->uiProgram);
}

static void draw_rect(struct engine* eng, float x, float y, float w, float h, float r, float g, float b) {
    float nx = (x/eng->width)*2-1, ny = 1-(y/eng->height)*2;
    float nw = w/eng->width, nh = h/eng->height;
    float v[] = { nx-nw, ny-nh, nx+nw, ny-nh, nx+nw, ny+nh, nx-nw, ny-nh, nx+nw, ny+nh, nx-nw, ny+nh };
    glUseProgram(eng->uiProgram);
    glUniform4f(glGetUniformLocation(eng->uiProgram, "col"), r, g, b, 1.0f);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, v);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
#endif
