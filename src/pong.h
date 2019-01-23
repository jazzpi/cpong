#ifndef __PONG_H_
#define __PONG_H_

#include <stdbool.h>

#define PONG_ROWS 28
#define PONG_COLS 28
#define PONG_PADDLE 3
#define PONG_PADDLE_MAX (PONG_COLS - PONG_PADDLE)
#define PONG_RANDOM_MAX (M_PI / 4)

typedef enum {
    PONG_NONE, PONG_UP, PONG_DOWN, PONG_QUIT
} pong_move;

typedef struct {
    float x;
    float y;
    float dir;
} pong_ball;

typedef struct {
    int paddles[2];
    int scores[2];
    pong_ball ball;
} pong;

void pong_init(pong* game);

void pong_reset_board(pong* game);

void pong_pmove(int* pos, int ydiff);

/**
 * Generate a random offset to the ball.
 */
float pong_random_offset(void);

/**
 * Collision check with a paddle.
 *
 * @param game The game handle
 * @param x, y The x/y coordinates of the ball
 * @param i    Which paddle to check against
 */
bool pong_cc_paddle(pong* game, float x, float y, int i);

void pong_bmove(pong* game);

bool pong_tick(pong* game, pong_move moves[2]);

#endif // __PONG_H_
