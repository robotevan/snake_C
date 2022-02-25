#include "stdio.h"
#include <stdlib.h>
#include "snake.h"
#include <unistd.h>
#include "pthread.h"

// screen
#define clrscr() printf("\e[1;1H\e[2J")
#define REFRESH_MS 350
#define REFRESH_PERIOD_US REFRESH_MS * 1000

// input
#define UP_CHAR    'w'
#define RIGHT_CHAR 'd'
#define DOWN_CHAR  's'
#define LEFT_CHAR  'a'
#define QUIT_CHAR  'q'

#define HEIGHT 15
#define WIDTH HEIGHT *2

typedef enum game_state{
    RUNNING,
    GAME_OVER,
} game_state_t;

typedef struct snake_point{
    int x;
    int y;
} snake_point_t;

typedef struct snake_game {
    int width;
    int height;
    int score;
    snake_t snake;
    game_state_t current_game_state;
    direction_t current_direction;
    snake_point_t current_snack;
} snake_game_t;

void new_snake_point_pos(snake_point_t *point, snake_t *snake, int max_width, int max_height){
    int rand_x, rand_y;
    do{
        rand_x = (rand()+1) % (max_width-2);
        rand_y = (rand()+1) % (max_width-2);
    }while(is_snake_block(snake, rand_x, rand_y));
    point->x = rand_x;
    point->y = rand_y;
}

int is_snake_at_point_block(snake_game_t *game){
    return (game->current_snack.x == game->snake.head->x &&
                game->current_snack.y == game->snake.head->y);
}

void is_snake_dead(snake_game_t *game){
    snake_t *s = &game->snake;
    if(s->head->x == 0 || s->head->x == game->width - 1){
        game->current_game_state = GAME_OVER;
    }else if(s->head->y == 0 || s->head->y == game->height - 1){
        game->current_game_state = GAME_OVER;
    }
    // final case, if snake hits itself, check if head hits any other
    snake_t tmp_snake;
    tmp_snake.head = s->head->next_block; // skip head
    tmp_snake.tail = s->tail;
    if(is_snake_block(&tmp_snake, s->head->x, s->head->y)){
        game->current_game_state = GAME_OVER;
    }
}

void create_game(snake_game_t *game, int width, int height){
    game->width = width;
    game->height = height;
    game->current_game_state = RUNNING;
    game->current_direction = RIGHT;
    game->score = 0;
    srand(time(NULL));
    create_snake(&game->snake, width/2, height/2);
    new_snake_point_pos(&game->current_snack, &game->snake, width, height);
}

void print_game_frame(snake_game_t *game){
    clrscr();
    // print top of frame
    for (int y = 0; y < game->height; y++){
        for (int x = 0; x < game->width; x++){
            if(x == game->width - 1){
                printf("#\n");
            }else if (x == 0){
                printf("#");
            }else if (is_snake_block(&game->snake, x, y)){
                printf("o");
            }else if (game->current_snack.x == x && game->current_snack.y == y){
                printf("@");
            }else if (y == 0 || y == game->height - 1){
                printf("#");
            }else{
                printf(" ");
            }
        }
    }
    printf("score: %d\n", game->score);
}

void move_snake(snake_game_t *game){
    // get current coords of head
    int curr_x = game->snake.head->x;
    int curr_y = game->snake.head->y;
    switch (game->current_direction) {
    case UP:
        add_snake_block(&game->snake, curr_x, curr_y - 1);
        break;
    
    case RIGHT:
        add_snake_block(&game->snake, curr_x + 1, curr_y);
        break;
    
    case DOWN:
        add_snake_block(&game->snake, curr_x, curr_y + 1);
        break;

    case LEFT:
        add_snake_block(&game->snake, curr_x - 1, curr_y);
        break;
    }
    // leave tail if snake ate point
    if(!is_snake_at_point_block(game)){
        remove_snake_tail(&game->snake);
    }else{
        new_snake_point_pos(&game->current_snack, &game->snake, game->width, game->height);
        game->score++;
    }
}

void update_game(snake_game_t *game){
    move_snake(game);
    is_snake_dead(game);
}

void *read_input(void *game){
    snake_game_t *g = (snake_game_t *)game;
    while(g->current_game_state == RUNNING){
        printf("Getting input\n");  
        char in_btn = getchar();
        switch (in_btn) {
        case UP_CHAR:
            g->current_direction = UP;
            break;
        case RIGHT_CHAR:
            g->current_direction = RIGHT;
            break;
        case DOWN_CHAR:
            g->current_direction = DOWN;
            break;
        case LEFT_CHAR:
            g->current_direction = UP;
            break;
        case QUIT_CHAR:
            g->current_game_state = GAME_OVER;
        }
    }
}

int main(){
    // create the game
    snake_game_t game; 
    create_game(&game, WIDTH, HEIGHT);
    // create the input listener
    pthread_t input_thread;
    //TODO : DEFINITELEY RACE CONDITION!
    pthread_create(&input_thread, NULL, read_input, (void *)&game);
    pthread_detach(input_thread);
    while(game.current_game_state == RUNNING){
        print_game_frame(&game);
        update_game(&game);
        usleep(REFRESH_PERIOD_US);
    }
    printf("GAME OVER!\n");
}