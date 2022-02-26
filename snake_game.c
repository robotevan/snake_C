#include "stdio.h"
#include <stdlib.h>
#include "snake.h"
#include <unistd.h>
#include "pthread.h"
#include "termios.h"

// screen
#define clrscr() printf("\e[1;1H\e[2J")
#define REFRESH_MS 500
#define REFRESH_PERIOD_US REFRESH_MS * 1000

// input
#define UP_CHAR    'w'
#define RIGHT_CHAR 'd'
#define DOWN_CHAR  's'
#define LEFT_CHAR  'a'
#define QUIT_ESC_CHAR  27

#define HEIGHT 15
#define WIDTH HEIGHT *2


/*
 * States for the snake game
 */
typedef enum game_state{
    RUNNING,
    GAME_OVER,
} game_state_t;

/*
 * snake point block
 * x : x coordinate from left to right
 * y : y coordinate from top to bottom
 */
typedef struct snake_point{
    int x;
    int y;
} snake_point_t;

/*
 * Holds the current content for the game
 * width : of board in characters, (#)
 * height : of board in characters (#)
 * score : of current game
 * snake : double linked list of snake_block_t
 * current_game_state : RUNNING / GAME_OVER
 * current_direction : UP / DOWN / LEFT / RIGHT
 * current snack : coords to current snack on board
 */
typedef struct snake_game {
    int width;
    int height;
    int score;
    snake_t snake;
    game_state_t current_game_state;
    direction_t current_direction;
    snake_point_t current_snack;
} snake_game_t;

/*
 * Create a new snack/point at a random location
 * @param game : reference to current game
 */
void new_snake_point_pos(snake_game_t *game){
    int rand_x, rand_y;
    do{
        rand_x = (rand()+1) % (game->width-2);
        rand_y = (rand()+1) % (game->height-2);
    }while(is_snake_block(&game->snake, rand_x, rand_y));
    game->current_snack.x = rand_x;
    game->current_snack.y = rand_y;
}

/*
 * check if snake head is at same coords as point block
 * @param reference to current game
 */
int is_snake_at_point_block(snake_game_t *game){
    return (game->current_snack.x == game->snake.head->x &&
                game->current_snack.y == game->snake.head->y);
}

/*
 * Check if snake has collided with any walls/itself
 * @param reference to current game
 */
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

/*
 * populate a game struct
 * @param reference to new game struct
 * @param width of new game
 * @param height of new game
 */
void create_game(snake_game_t *game, int width, int height){
    game->width = width;
    game->height = height;
    game->current_game_state = RUNNING;
    game->current_direction = RIGHT;
    game->score = 0;
    srand(time(NULL));
    create_snake(&game->snake, width/2, height/2);
    new_snake_point_pos(game);
}

void init_input_listener(){
    struct termios info;
    tcgetattr(0, &info);          // populate struct with current info
    info.c_lflag &= ~ICANON;      // disable canonical mode
    info.c_cc[VMIN] = 1;          // listen for 1 keystroke
    info.c_cc[VTIME] = 0;         // no timeouit, prevent blocking
    tcsetattr(0, TCSANOW, &info); // set new config
}

/*
 * print the contents of the current game including snake,
 * boarders, snack and score
 * @param reference to current game
 */
void print_game_frame(snake_game_t *game){
    clrscr(); // TODO : NOT WORKING
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

/*
 * Update the snakes current direction, prevent from turning 180deg
 * @param reference to current game
 * @param new desired direction
 */
void set_snake_directtion(snake_game_t *game, direction_t new_direction){
    //TODO : Mutex lock! Will be accessed from control thread
    if(new_direction != game->current_direction){
        game->current_direction = new_direction;
    }
}

/*
 * move snake by 1 position depending on direction
 * @param reference to current game
 */
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
    // remove tail to simulate moving
    if(!is_snake_at_point_block(game)){
        remove_snake_tail(&game->snake);
    }else{ // if at point, leave tail (grow) and increment score
        new_snake_point_pos(game);
        game->score+=10;
    }
}

/*
 * Call every frame to update the game, including move,
 * check if alive, check if point.....
 * @param reference to current game
 */
void update_game(snake_game_t *game){
    move_snake(game);
    is_snake_dead(game);
}

/*
 * seperate thread to read user inputs
 */
void *read_input(void *game){
    init_input_listener(); // setup input
    snake_game_t *g = (snake_game_t *)game;
    int ch;
    while((ch = getchar()) != QUIT_ESC_CHAR && g->current_game_state == RUNNING){
        if (ch < 0){
            perror("Failed to read stdin");
            exit(EXIT_FAILURE);
        }
        switch (ch) {
        case UP_CHAR:
            set_snake_directtion(g, UP);
            break;
        case RIGHT_CHAR:
            set_snake_directtion(g, RIGHT);
            break;
        case DOWN_CHAR:
            set_snake_directtion(g, DOWN);
            break;
        case LEFT_CHAR:
            set_snake_directtion(g, LEFT);
            break;
        case QUIT_ESC_CHAR:
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