#ifndef INPUT_H
#define INPUT_H

#include "structs.h"

void input_init(Game* g);
void input_poll(Game* g);
bool key_pressed(Game* g, SDL_Scancode sc);   // edge (down this frame)
bool key_down(Game* g, SDL_Scancode sc);      // held

#endif // INPUT_H

