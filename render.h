#ifndef RENDER_H
#define RENDER_H
#include "engine.h"

static GLuint load_shader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);
    return s;
}

static void init_render(struct engine* eng) {
    const char* vW = "attribute vec4 p; attribute vec3 n; uniform mat4 m; varying float vL; void main(){ gl_Position=m*p; vL=max(dot(n, vec3(0.3, 1.0, 0.5)), 0.2); }";
    const char* fW = "precision mediump float; varying float vL; void main(){ gl_FragColor=vec4(0.4*vL, 0.7*vL, 1.0*vL, 1.0); }";
    
    eng->program = glCreateProgram();
    glAttachShader(eng->program, load_shader(GL_VERTEX_SHADER, vW));
    glAttachShader(eng->program, load_shader(GL_FRAGMENT_SHADER, fW));
    glLinkProgram(eng->program);

    const char* vU = "attribute vec4 p; void main(){ gl_Position=p; }";
    const char* fU = "precision mediump float; uniform vec4 c; void main(){ gl_FragColor=c; }";
    eng->uiProgram = glCreateProgram();
    glAttachShader(eng->uiProgram, load_shader(GL_VERTEX_SHADER, vU));
    glAttachShader(eng->uiProgram, load_shader(GL_FRAGMENT_SHADER, fU));
    glLinkProgram(eng->uiProgram);
}

static void draw_ui_box(struct engine* eng, float x, float y, float w, float h, float r, float g, float b, float a) {
    float nx = (x/eng->width)*2.0f-1.0f, ny = 1.0f-(y/eng->height)*2.0f;
    float nw = w/eng->width, nh = h/eng->height;
    float v[] = {nx-nw,ny-nh, nx+nw,ny-nh, nx+nw,ny+nh, nx-nw,ny-nh, nx+nw,ny+nh, nx-nw,ny+nh};
    glUseProgram(eng->uiProgram);
    glUniform4f(glGetUniformLocation(eng->uiProgram, "c"), r, g, b, a);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, v);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
#endif
