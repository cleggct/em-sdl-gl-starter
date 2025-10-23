#include "play.h"
#include "graphics.h"
#include "input.h"
#include "defs.h"

// Local play state data (static, for skeleton)
static Texture* g_tex = NULL;
static Atlas*   g_atlas = NULL;
static Sprite   player;

static void enter(Game* g) {
    SDL_Log("Enter PLAY");
    if (!g_tex) {
        g_tex = texture_load(g->gfx, SPRITESHEET_PNG);
        if (!g_tex) SDL_Log("WARNING: failed to load %s; using blank", SPRITESHEET_PNG);
        g_atlas = g_tex ? atlas_create(g_tex, ATLAS_TILE_W, ATLAS_TILE_H) : NULL;
    }
    player.atlas = g_atlas;
    player.tile_index = 0;
    player.x = GAME_WIDTH/2 - ATLAS_TILE_W/2;
    player.y = GAME_HEIGHT - ATLAS_TILE_H - 12;
    player.tint = (SDL_Color){255,255,255,255};
    player.flip = SDL_FLIP_NONE;
}

static void exit_(Game* g) { (void)g; SDL_Log("Exit PLAY"); }

static void handle_input(Game* g) {
    int speed = 200; // px/sec
    if (key_down(g, SDL_SCANCODE_LEFT))  player.x -= (int)(speed * (g->dt_ms / 1000.0f));
    if (key_down(g, SDL_SCANCODE_RIGHT)) player.x += (int)(speed * (g->dt_ms / 1000.0f));
    player.x = CLAMP(player.x, 0, GAME_WIDTH - ATLAS_TILE_W);
}

static void update(Game* g, float dt) { (void)g; (void)dt; }

static void render(Game* g) {
    draw_clear(g->gfx, 16,16,24,255);
    draw_sprite(g->gfx, &player);
    SDL_RenderPresent(g->renderer);
}

State PLAY_STATE = {
    .name = "PLAY",
    .enter = enter,
    .exit  = exit_,
    .handle_input = handle_input,
    .update = update,
    .render = render
};

