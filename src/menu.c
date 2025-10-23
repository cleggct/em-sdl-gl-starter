#include "menu.h"
#include "graphics.h"
#include "state.h"
#include "play.h"
#include "input.h"
#include "defs.h"

static void enter(Game* g)        { (void)g; SDL_Log("Enter MENU"); }
static void exit_(Game* g)        { (void)g; SDL_Log("Exit MENU");  }
static void handle_input(Game* g) {
    if (key_pressed(g, SDL_SCANCODE_RETURN) || key_pressed(g, SDL_SCANCODE_SPACE)) {
        state_stack_push(g->states, g, &PLAY_STATE);
    }
}
static void update(Game* g, float dt) { (void)g; (void)dt; }
static void render(Game* g) {
    draw_clear(g->gfx, 20,20,20,255);
    graphics_present(g->gfx);
}

State MENU_STATE = {
    .name = "MENU",
    .enter = enter,
    .exit  = exit_,
    .handle_input = handle_input,
    .update = update,
    .render = render
};

