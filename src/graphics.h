#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "structs.h"

typedef struct Graphics Graphics;

// create/destroy
Graphics* graphics_create(SDL_Window* window);
void      graphics_destroy(Graphics* g);

// textures & atlases
Texture* texture_load(Graphics* g, const char* path);
void     texture_free(Texture* t);
Atlas*   atlas_create(Texture* tex, int tile_w, int tile_h);
void     atlas_free(Atlas* a);

// drawing
void draw_clear(Graphics* g, uint8_t r, uint8_t gC, uint8_t b, uint8_t a);
void draw_sprite(Graphics* g, const Sprite* s);
void graphics_present(Graphics* g); // swap buffers

#endif // GRAPHICS_H

