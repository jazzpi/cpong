#include "pong.h"

#include <math.h>
#include <stdlib.h>

void pong_init(pong* game) {
    game->scores[0] = game->scores[1] = 0;

    pong_reset_board(game);
}

void pong_reset_board(pong* game) {
    game->paddles[0] = game->paddles[1] = PONG_ROWS / 2;
    game->ball.x = PONG_COLS / 2.0;
    game->ball.y = PONG_ROWS / 2.0;
    game->ball.dir = 0;
}

void pong_pmove(int* pos, int ydiff) {
    *pos = *pos + ydiff;
    *pos = (*pos < 0) ? 0 : *pos;
    *pos = (*pos > PONG_PADDLE_MAX) ? PONG_PADDLE_MAX : *pos;
}

bool pong_cc_paddle(pong* game, float x, float y, int i) {
    int xi = round(x);
    if ((xi < 0 && i == 0) || (xi == PONG_COLS && i == 1)) {
        int yi = round(y);
        return (game->paddles[i] <= yi) &&
            (yi < game->paddles[i] + PONG_PADDLE);
    }

    return false;
}

float pong_random_offset(void) {
    int r = rand();
    /* Interpret as a random float */
    float f = *((float*) &r);
    return fmod(f, PONG_RANDOM_MAX);
}

void pong_bmove(pong* game) {
    float x = game->ball.x + cos(game->ball.dir);
    float y = game->ball.y + sin(game->ball.dir);


    if (y < 0 || round(y) >= PONG_ROWS) {
        game->ball.dir = -game->ball.dir;
        pong_bmove(game);
        return;
    }

    if (pong_cc_paddle(game, x, y, 0) || pong_cc_paddle(game, x, y, 1)) {
        game->ball.dir = fmod(game->ball.dir + M_PI + pong_random_offset(),
                              2 * M_PI);
        pong_bmove(game);
        return;
    }

    game->ball.x = x;
    game->ball.y = y;
}

bool pong_tick(pong* game, pong_move moves[2]) {
    int i;
    for (i = 0; i < 2; i++) {
        switch (moves[i]) {
        case PONG_QUIT:
            return false;
        case PONG_UP:
            pong_pmove(&game->paddles[i], -1);
            break;
        case PONG_DOWN:
            pong_pmove(&game->paddles[i], 1);
            break;
        default:
            break;
        }
    }

    pong_bmove(game);

    int xi = round(game->ball.x);
    if (xi < -1) {
        game->scores[0]++;
    } else if (xi > PONG_COLS) {
        game->scores[1]++;
    }
    return xi >= -1 && xi <= PONG_COLS;
}
