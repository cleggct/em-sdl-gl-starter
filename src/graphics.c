#include "graphics.h"
#include <SDL2/SDL_image.h>
#include <SDL_opengles2.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct Texture { GLuint gl; int w, h; };

struct Graphics {
    SDL_Window* win;
    SDL_GLContext gl;
    int w, h;

    // GL pipeline
    GLuint prog, vbo, ibo;
    GLint  u_proj, u_tint, u_tex;
};

static const char* VS_SRC =
    "attribute vec2 a_pos;\n"
    "attribute vec2 a_uv;\n"
    "uniform mat4 u_proj;\n"
    "varying vec2 v_uv;\n"
    "void main(){ gl_Position = u_proj * vec4(a_pos,0.0,1.0); v_uv = a_uv; }\n";

static const char* FS_SRC =
    "precision mediump float;\n"
    "varying vec2 v_uv;\n"
    "uniform sampler2D u_tex;\n"
    "uniform vec4 u_tint;\n"
    "void main(){ gl_FragColor = texture2D(u_tex, v_uv) * u_tint; }\n";

static GLuint compile(GLenum type, const char* src){
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);
    GLint ok=0; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if(!ok){ char log[512]; glGetShaderInfoLog(s, 512, NULL, log); SDL_Log("shader: %s", log); }
    return s;
}
static GLuint link(GLuint vs, GLuint fs){
    GLuint p = glCreateProgram();
    glAttachShader(p, vs); glAttachShader(p, fs);
    glBindAttribLocation(p, 0, "a_pos");
    glBindAttribLocation(p, 1, "a_uv");
    glLinkProgram(p);
    GLint ok=0; glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if(!ok){ char log[512]; glGetProgramInfoLog(p, 512, NULL, log); SDL_Log("program: %s", log); }
    glDeleteShader(vs); glDeleteShader(fs);
    return p;
}
static void ortho(float l,float r,float b,float t,float* M){
    float n=-1.f,f=1.f;
    float tx=-(r+l)/(r-l), ty=-(t+b)/(t-b), tz=-(f+n)/(f-n);
    float sx= 2.f/(r-l),   sy= 2.f/(t-b),   sz=-2.f/(f-n);
    M[0]=sx; M[4]=0;  M[8]=0;  M[12]=tx;
    M[1]=0;  M[5]=sy; M[9]=0;  M[13]=ty;
    M[2]=0;  M[6]=0;  M[10]=sz;M[14]=tz;
    M[3]=0;  M[7]=0;  M[11]=0; M[15]=1;
}

Graphics* graphics_create(SDL_Window* window){
    Graphics* g = (Graphics*)calloc(1, sizeof(Graphics));
    g->win = window;
    // If init.c already created a GL context, use it.
    g->gl = SDL_GL_GetCurrentContext();
    if(!g->gl){
        // Create one if not present (sane defaults)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        g->gl = SDL_GL_CreateContext(window);
        SDL_GL_SetSwapInterval(1);
    }
    SDL_GL_GetDrawableSize(window, &g->w, &g->h);

    GLuint vs = compile(GL_VERTEX_SHADER, VS_SRC);
    GLuint fs = compile(GL_FRAGMENT_SHADER, FS_SRC);
    g->prog = link(vs, fs);
    g->u_proj = glGetUniformLocation(g->prog, "u_proj");
    g->u_tint = glGetUniformLocation(g->prog, "u_tint");
    g->u_tex  = glGetUniformLocation(g->prog, "u_tex");

    // Static index buffer
    const GLushort idx[] = {0,1,2, 0,2,3};
    glGenBuffers(1, &g->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

    // Dynamic VBO (we update per sprite)
    glGenBuffers(1, &g->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, g->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*4, NULL, GL_DYNAMIC_DRAW);

    glViewport(0,0,g->w,g->h);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glClearColor(0.08f,0.08f,0.10f,1.0f);

    return g;
}

void graphics_destroy(Graphics* g){
    if(!g) return;
    if(g->prog) glDeleteProgram(g->prog);
    if(g->vbo)  glDeleteBuffers(1,&g->vbo);
    if(g->ibo)  glDeleteBuffers(1,&g->ibo);
    if(g->gl){ SDL_GL_DeleteContext(g->gl); g->gl=NULL; }
    free(g);
}

static GLuint make_tex_rgba(const void* pixels, int w, int h){
    GLuint t=0;
    glGenTextures(1,&t);
    glBindTexture(GL_TEXTURE_2D,t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    return t;
}

Texture* texture_load(Graphics* g, const char* path){
    (void)g;
    SDL_Surface* s = IMG_Load(path);
    if(!s){ SDL_Log("IMG_Load %s: %s", path, IMG_GetError()); return NULL; }
    SDL_Surface* conv = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(s);
    if(!conv){ SDL_Log("ConvertSurface failed"); return NULL; }

    Texture* T = (Texture*)calloc(1,sizeof(Texture));
    T->w = conv->w; T->h = conv->h;
    T->gl = make_tex_rgba(conv->pixels, conv->w, conv->h);
    SDL_FreeSurface(conv);
    return T;
}

void texture_free(Texture* t){
    if(!t) return;
    if(t->gl) glDeleteTextures(1,&t->gl);
    free(t);
}

Atlas* atlas_create(Texture* tex, int tile_w, int tile_h){
    Atlas* a = (Atlas*)calloc(1,sizeof(Atlas));
    a->tex = tex;
    a->tile_w = tile_w; a->tile_h = tile_h;
    a->cols = tex->w / tile_w;
    a->rows = tex->h / tile_h;
    return a;
}
void atlas_free(Atlas* a){ free(a); }

void draw_clear(Graphics* g, uint8_t r, uint8_t gC, uint8_t b, uint8_t a){
    glClearColor(r/255.0f, gC/255.0f, b/255.0f, a/255.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void draw_sprite(Graphics* g, const Sprite* s){
    if(!s || !s->atlas || !s->atlas->tex) return;

    const int cols = s->atlas->cols;
    const int cx = s->tile_index % cols;
    const int cy = s->tile_index / cols;

    const int tw = s->atlas->tile_w, th = s->atlas->tile_h;
    float x = (float)s->x, y = (float)s->y;

    // Build quad vertices (pos.xy, uv.xy)
    float u0 = (float)(cx * tw) / (float)s->atlas->tex->w;
    float v0 = (float)(cy * th) / (float)s->atlas->tex->h;
    float u1 = (float)((cx+1) * tw) / (float)s->atlas->tex->w;
    float v1 = (float)((cy+1) * th) / (float)s->atlas->tex->h;

    // handle flip bits (0x1 X, 0x2 Y)
    const int flipX = (s->flip & 1) != 0;
    const int flipY = (s->flip & 2) != 0;
    if (flipX) { float t=u0; u0=u1; u1=t; }
    if (flipY) { float t=v0; v0=v1; v1=t; }

    float verts[16] = {
        x,     y,     u0, v0,
        x+tw,  y,     u1, v0,
        x+tw,  y+th,  u1, v1,
        x,     y+th,  u0, v1,
    };

    // Upload vertices
    glBindBuffer(GL_ARRAY_BUFFER, g->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);

    glUseProgram(g->prog);

    // Ortho projection 0..w,0..h
    float P[16]; ortho(0,(float)g->w,(float)g->h,0,P);
    glUniformMatrix4fv(g->u_proj, 1, GL_FALSE, P);

    glUniform4f(g->u_tint, s->tint.r/255.f, s->tint.g/255.f, s->tint.b/255.f, s->tint.a/255.f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, s->atlas->tex->gl);
    glUniform1i(g->u_tex, 0);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (const void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (const void*)(sizeof(float)*2));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g->ibo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}

void graphics_present(Graphics* g){
    SDL_GL_SwapWindow(g->win);
}
