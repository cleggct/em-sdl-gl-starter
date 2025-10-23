#include "input.h"

void input_init(Game* g) {
    g->input.kb = SDL_GetKeyboardState(NULL);
    memset(g->input.prev_keys, 0, sizeof(g->input.prev_keys));
    g->input.quit_requested = false;
}

void input_poll(Game* g) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) g->input.quit_requested = true;
    }

    // edge-detection: copy current to prev at end of frame; but we need prev from LAST frame.
    // So we’ll fill prev now and rely on SDL’s pointer staying valid.
    memcpy(g->input.prev_keys, g->input.kb, sizeof(g->input.prev_keys));

    // Refresh current keyboard snapshot pointer (SDL maintains it)
    SDL_PumpEvents();
    g->input.kb = SDL_GetKeyboardState(NULL);

    // Optional: ESC to quit
    if (g->input.kb[SDL_SCANCODE_ESCAPE]) g->input.quit_requested = true;
}

bool key_down(Game* g, SDL_Scancode sc) {
    return g->input.kb[sc] != 0;
}

bool key_pressed(Game* g, SDL_Scancode sc) {
    return g->input.kb[sc] && !g->input.prev_keys[sc];
}

