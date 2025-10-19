#include <SDL.h>
#ifdef __EMSCRIPTEN__
  #include <emscripten/emscripten.h>
#endif

static SDL_Window*   win   = NULL;
static SDL_Renderer* ren   = NULL;
static SDL_Texture*  tex   = NULL;
static int           running = 1;

static void frame(void* userdata) {
    (void)userdata;

    // handle events
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = 0;
#ifdef __EMSCRIPTEN__
            emscripten_cancel_main_loop(); // stop RAF
#endif
            return;
        }
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
            running = 0;
#ifdef __EMSCRIPTEN__
            emscripten_cancel_main_loop();
#endif
            return;
        }
    }

    // draw
    SDL_SetRenderDrawColor(ren, 16,16,16,255);
    SDL_RenderClear(ren);

    if (tex) {
        SDL_Rect dst = { 100, 80, 128, 128 };
        SDL_RenderCopy(ren, tex, NULL, &dst);
    }

    SDL_RenderPresent(ren);
}

int main(void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) return 1;

    win = SDL_CreateWindow("SDL demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                           800, 450, SDL_WINDOW_SHOWN);
    if (!win) return 1;

    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) return 1;

    // TODO: load tex if you want (or leave NULL)

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(frame, NULL, 0, 1);
#else
    while (running) {
        frame(NULL);
        // small sleep to avoid pegging CPU if vsync is off
        SDL_Delay(1);
    }
#endif

    // cleanup
    if (tex) SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}

