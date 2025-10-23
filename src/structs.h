#ifndef STRUCTS_H
#define STRUCTS_H

#include "common.h"
#include <SDL.h>

typedef struct Texture Texture;

// Forward declarations
struct Graphics;

// Sprite atlas: uniform grid of tiles
typedef struct Atlas {
  Texture* tex;
  int tile_w, tile_h;
  int cols, rows;
} Atlas;

// Basic sprite draw info
typedef struct Sprite {
  Atlas* atlas;
  int    tile_index;
  int    x, y;
  SDL_Color tint;
  uint8_t   flip;  // 1=X, 2=Y
} Sprite;

// Input snapshot
typedef struct Input {
    const uint8_t* kb;   // SDL keyboard state
    bool quit_requested;
    // edge detection buffers (optional)
    uint8_t prev_keys[SDL_NUM_SCANCODES];
} Input;

// Game-wide state forward decl (owned in main.c)
typedef struct Game {
    SDL_Window* window;
    SDL_Renderer* renderer;

    // modules
    struct Graphics* gfx;
    Input input;

    // timing
    uint64_t perf_freq;
    uint64_t last_ticks;
    float dt_ms;

    // game state stack
    struct StateStack* states;

    bool running;
} Game;

// State interface
typedef struct State {
    const char* name;
    void (*enter)(Game* g);
    void (*exit)(Game* g);
    void (*handle_input)(Game* g); // optional, usually empty
    void (*update)(Game* g, float dt_ms);
    void (*render)(Game* g);
} State;

#endif // STRUCTS_H

