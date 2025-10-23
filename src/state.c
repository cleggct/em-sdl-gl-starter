#include "state.h"

void state_stack_init(StateStack* s) {
    s->top = -1;
}

bool state_stack_push(StateStack* s, Game* g, State* st) {
    if (s->top + 1 >= STATE_STACK_MAX) return false;
    s->items[++s->top] = st;
    if (st->enter) st->enter(g);
    return true;
}

void state_stack_pop(StateStack* s, Game* g) {
    if (s->top < 0) return;
    State* st = s->items[s->top--];
    if (st && st->exit) st->exit(g);
}

State* state_stack_peek(StateStack* s) {
    if (s->top < 0) return NULL;
    return s->items[s->top];
}

