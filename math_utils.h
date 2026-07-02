#ifndef MATH_UTILS_H
#define MATH_UTILS_H
#include <string.h>
#include <math.h>

static void mat4_identity(float* m) {
    memset(m, 0, 64); m[0]=m[5]=m[10]=m[15]=1.0f;
}
static void mat4_mul(float* out, float* a, float* b) {
    float res[16];
    for (int c=0; c<4; c++) for (int r=0; r<4; r++)
        res[c*4+r] = a[0*4+r]*b[c*4+0] + a[1*4+r]*b[c*4+1] + a[2*4+r]*b[c*4+2] + a[3*4+r]*b[c*4+3];
    memcpy(out, res, 64);
}
static void mat4_perspective(float* m, float fov, float aspect, float n, float f) {
    float S = 1.0f / tanf(fov * 0.5f);
    memset(m, 0, 64); m[0]=S/aspect; m[5]=S; m[10]=(f+n)/(n-f); m[11]=-1; m[14]=(2*f*n)/(n-f);
}
static void mat4_lookat(float* m, float* pos, float pitch, float yaw) {
    float cp = cosf(pitch), sp = sinf(pitch), cy = cosf(yaw), sy = sinf(yaw);
    float f[3] = { sy*cp, -sp, -cy*cp }, r[3] = { cy, 0, sy }, u[3] = { sy*sp, cp, -cy*sp };
    mat4_identity(m);
    m[0]=r[0]; m[4]=r[1]; m[8]=r[2]; m[1]=u[0]; m[5]=u[1]; m[9]=u[2]; m[2]=-f[0]; m[6]=-f[1]; m[10]=-f[2];
    m[12]=-(r[0]*pos[0]+r[1]*pos[1]+r[2]*pos[2]); m[13]=-(u[0]*pos[0]+u[1]*pos[1]+u[2]*pos[2]); m[14]=(f[0]*pos[0]+f[1]*pos[1]+f[2]*pos[2]);
}
#endif
