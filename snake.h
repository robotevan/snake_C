#ifndef SNAKE_H
#define SNAKE_H

typedef enum direction{
    UP,
    RIGHT,
    DOWN,
    LEFT
} direction_t;

typedef struct snake_block{
    int x;
    int y;
    struct snake_block *next_block;
    struct snake_block *prev_block;
} snake_block_t;

typedef struct snake {
    struct snake_block *head;
    struct snake_block *tail;
} snake_t;

void create_snake(snake_t *snake, int start_x, int start_y);
void add_snake_block(snake_t *snake, int x, int y);
void remove_snake_tail(snake_t *snake);
void move_snake(snake_t *snake, direction_t direction);
int is_snake_block(snake_t *snake, int x, int y);

#endif