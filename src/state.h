#ifndef STATE_H
#define STATE_H

#include "structs.h"

// simple fixed-size stack
#define STATE_STACK_MAX 8

typedef struct StateStack {
    State* items[STATE_STACK_MAX];
    int top; // -1 when empty
} StateStack;

void state_stack_init(StateStack* s);
bool state_stack_push(StateStack* s, Game* g, State* st);
void state_stack_pop(StateStack* s, Game* g);
State* state_stack_peek(StateStack* s);

#endif // STATE_H

