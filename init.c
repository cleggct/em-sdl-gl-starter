#include "init.h"
#include "defs.h"
#include <SDL2/SDL_image.h>

bool init_sdl(Game* g) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return false;
    }
    int imgFlags = IMG_INIT_PNG;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
        SDL_Log("IMG_Init failed: %s", IMG_GetError());
        return false;
    }

    // Request OpenGL ES 2.0 (WebGL1-friendly)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    g->window = SDL_CreateWindow(GAME_TITLE,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        GAME_WIDTH, GAME_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!g->window) {
        SDL_Log("CreateWindow failed: %s", SDL_GetError());
        return false;
    }

    // Create context now (graphics_create will use it)
    SDL_GLContext gl = SDL_GL_CreateContext(g->window);
    if (!gl) {
        SDL_Log("SDL_GL_CreateContext failed: %s", SDL_GetError());
        return false;
    }
    SDL_GL_SetSwapInterval(1);

    g->perf_freq = SDL_GetPerformanceFrequency();
    g->last_ticks = SDL_GetPerformanceCounter();
    g->dt_ms = 0.0f;
    g->running = true;
    return true;
}

void shutdown_sdl(Game* g) {
    // GL context destroyed in graphics_destroy
    if (g->window)   { SDL_DestroyWindow(g->window);     g->window = NULL; }
    IMG_Quit();
    SDL_Quit();
}

