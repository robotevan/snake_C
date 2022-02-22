#include "stdio.h"
#include <stdio.h>
#include "stdlib.h"
#include "snake.h"

void create_snake(snake_t *snake, int start_x, int start_y){
    snake->head = NULL;
    snake->tail = NULL;
    add_snake_block(snake, start_x, start_y);
}

struct snake_block *create_snake_block(int x, int y){
    struct snake_block *block = (struct snake_block *)malloc(sizeof(struct snake_block ));
    block->x = x;
    block->y = y;
    return block;
}

void add_snake_block(snake_t *snake, int x, int y){
    struct snake_block *new_head = create_snake_block(x, y);
    new_head->prev_block = NULL;
    if(snake->head == NULL){ // if empty
        snake->tail = new_head;
    }else if(snake->head == snake->tail) {
        snake->tail = snake->head;
        snake->tail->prev_block = snake->head;
    }
    new_head->next_block = snake->head;
    snake->head = new_head;
}

void remove_snake_tail(snake_t *snake){
    if(snake->head == NULL || snake->tail == NULL){ // size == 0
        return;
    }else if(snake->head == snake->tail){ // size == 1
        free(snake->head);
        snake->head == NULL;
        snake->tail == NULL;
    }else { // any other case
        snake->tail = snake->tail->prev_block;
        free(snake->tail->next_block);
        snake->tail->next_block = NULL;
    }
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

void print_snake(snake_t *snake){
    struct snake_block *tmp = snake->head;
    while(tmp->next_block != NULL){
        printf("[%d, %d]->", tmp->x, tmp->y);
        tmp = tmp->next_block;
    }
    printf("\n");
}