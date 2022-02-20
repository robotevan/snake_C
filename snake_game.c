#include "stdio.h"
#include "snake.h"

#define clrscr() printf("\e[1;1H\e[2J")

#define HEIGHT 15
#define WIDTH HEIGHT *2

typedef enum game_state{
    WAIT,
    RUNNING,
    GAME_OVER,
} game_state_t;

typedef struct snake_game {
    int width;
    int height;
    snake_t snake;
    game_state_t current_game_state;
} snake_game_t;

void create_game(snake_game_t *game, int width, int height){
    game->width = width;
    game->height = height;
    game->current_game_state = RUNNING;
    create_snake(&game->snake, width/2, height/2);
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
            }else if (is_snake_block(&game->snake, x, y) && y != 0 && y!=game->height-1){
                printf("o");
            }else if (y == 0 || y == game->height - 1){
                printf("#");
            }else{
                printf(" ");
            }
        }
    }
}

int main(){
    snake_game_t game; 
    create_game(&game, WIDTH, HEIGHT);
    print_game_frame(&game);
}