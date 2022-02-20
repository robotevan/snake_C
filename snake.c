#include "stdio.h"
#include <stdio.h>
#include "stdlib.h"
#include "snake.h"

void create_snake(snake_t *snake, int start_x, int start_y){
    snake->head = NULL;
    snake->tail = NULL;
    add_snake_block(snake, start_x, start_y);
}


void add_snake_block(snake_t *snake, int x, int y){
    struct snake_block *new_head = (struct snake_block *)malloc(sizeof(struct snake_block));
    new_head->prev_block = NULL;
    new_head->x = x;
    new_head->y = y;
    // is empty?
    if(snake->head == NULL){
        snake->head = new_head;
        snake->head->next_block = NULL; 
        snake->tail = snake->head; // size 1
    }else {
        struct snake_block *old_head = snake->head;
        snake->head = new_head;
        snake->head->next_block = new_head;
        old_head->prev_block = new_head;
    }
}

void remove_snake_tail(snake_t *snake){
    struct snake_block *old_tail = snake->tail;
    snake->tail = old_tail->prev_block;
    snake->tail->next_block = NULL;
    free(old_tail);
}

void move_snake(snake_t *snake, direction_t direction){
    int curr_x = snake->head->x;
    int curr_y = snake->head->y;
    switch (direction) {
    case UP:
        add_snake_block(snake, curr_x, curr_y - 1);
        break;
    
    case RIGHT:
        add_snake_block(snake, curr_x + 1, curr_y);
        break;
    
    case DOWN:
        add_snake_block(snake, curr_x, curr_y + 1);
        break;

    case LEFT:
        add_snake_block(snake, curr_x - 1, curr_y);
        break;
    }
    remove_snake_tail(snake);    
}

int is_snake_block(snake_t *snake, int x, int y){
    struct snake_block *tmp = snake->head;
    while(tmp != NULL){
        if (tmp->x == x && tmp->y == y){
            return 1;
        }
        tmp = tmp->next_block;
    }
    return 0;
}

