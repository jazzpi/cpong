#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "pong.h"
#include "ansi_esc.h"

#define TICK_RATE (1E6 / 20) /* us */

/* https://stackoverflow.com/a/37872060/2192641 */
void stdin_set(bool enable_nonblocking) {
    struct termios t;

    tcgetattr(1, &t);
    if (enable_nonblocking) {
        t.c_lflag &= ~(ICANON | ECHO);
        ansi_esc_show_cursor(false);
    } else {
        t.c_lflag |= ICANON | ECHO;
        ansi_esc_show_cursor(true);
    }
    tcsetattr(1, 0, &t);
}

void reset_terminal_mode(void) {
    stdin_set(false);
}

int kbhit() {
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

int getch() {
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}

static unsigned running = 1;

void handle_sigint(int dummy __attribute__((unused))) {
    running = 0;
}

static pong_move* read_chars(void) {
    static pong_move moves[2];
    moves[0] = moves[1] = PONG_NONE;

    static char buf[3];
    static unsigned index = 0;

    int r;
    while ((r = kbhit())) {
        assert(r > 0);
        buf[index] = getch();
        switch (index) {
        case 0:
            switch (buf[index]) {
            case '\033':
                index++;
                break;
            case 'Q': /* FALL-THROUGH */
            case 'q':
                moves[0] = PONG_QUIT;
                moves[1] = PONG_QUIT;
                break;
            case 'W': /* FALL-THROUGH */
            case 'w':
                moves[0] = PONG_UP;
                break;
            case 'S': /* FALL-THROUGH */
            case 's':
                moves[0] = PONG_DOWN;
                break;
            }
            break;
        case 1:
            if (buf[index] == '[') {
                index++;
            } else {
                index = 0;
            }
            break;
        case 2:
            switch (buf[index]) {
            case 'A':
                index = 0;
                moves[1] = PONG_UP;
                break;
            case 'B':
                index = 0;
                moves[1] = PONG_DOWN;
                break;
            }
            index = 0;
            break;
        }
    }
    return moves;
}

static void pong_move_cursor(int x, int y) {
    ansi_esc_move_cursor(x + 1, y + 1);
}

static void d_init(void) {
    ansi_esc_clear_screen();

    int i;
    for (i = 0; i <= PONG_COLS + 1; i++) {
        printf("▄");
    }
    ansi_esc_move_cursor(0, PONG_ROWS + 1);
    for (i = 0; i <= PONG_COLS + 1; i++) {
        printf("▀");
    }

    fflush(stdout);
}

static void d_paddle(int i, unsigned pos, unsigned prev) {
    int x = i ? PONG_COLS + 1 : 0;
    int y;
    for (y = 0; y < PONG_PADDLE; y++) {
        ansi_esc_move_cursor(x, prev + y + 1);
        printf(" ");
    }
    for (y = 0; y < PONG_PADDLE; y++) {
        ansi_esc_move_cursor(x, pos + y + 1);
        printf("█");
    }
}

static void d_ball(pong_ball ball, pong_ball prev) {
    pong_move_cursor(round(prev.x), round(prev.y));
    printf(" ");
    pong_move_cursor(round(ball.x), round(ball.y));
    printf("O");
}

static void d_score(pong* game) {
    ansi_esc_move_cursor((PONG_COLS - PONG_SCORE_STRLEN) / 2, 0);
    printf("%02d | %02d", game->scores[0], game->scores[1]);
}

static void d_game(pong* game, pong* prev) {
    int i;
    if (round(game->ball.x) != round(prev->ball.x) ||
        round(game->ball.y) != round(prev->ball.y)) {
        d_ball(game->ball, prev->ball);
    }
    for (i = 0; i < 2; i++) {
        if (game->paddles[i] != prev->paddles[i]) {
            d_paddle(i, game->paddles[i], prev->paddles[i]);
        }
    }
    d_score(game);
    fflush(stdout);
}

static pong game;
static pong prev;

const char* const PONG_GAME_OVER_HEADER_LINES[] = {
    "█▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀█",
    "▌ G A M E   O V E R ▐",
    "▌                   ▐",
    "▌   THE WINNER IS   ▐",
    NULL
};
#define PONG_GAME_OVER_WINNER_LINE  "▌      PLAYER %d     ▐"
#define PONG_GAME_OVER_FOOTER_LINE "█▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█"
#define PONG_GAME_OVER_WIDTH 22
#define PONG_GAME_OVER_HEIGHT 6

static void d_gameover(int player) {
    unsigned x = (PONG_COLS + 2 - PONG_GAME_OVER_WIDTH) / 2;
    unsigned y = (PONG_ROWS + 2 - PONG_GAME_OVER_HEIGHT) / 2;

    unsigned i;
    const char* line;
    for (i = 0; (line = PONG_GAME_OVER_HEADER_LINES[i]) != NULL; i++) {
        ansi_esc_move_cursor(x, y);
        printf("%s", line);
        y++;
    }
    ansi_esc_move_cursor(x, y);
    printf(PONG_GAME_OVER_WINNER_LINE, player);
    y++;
    ansi_esc_move_cursor(x, y);
    printf(PONG_GAME_OVER_FOOTER_LINE);
}

int main() {
    srand(time(NULL));
    signal(SIGINT, handle_sigint);
    stdin_set(true);

    pong_init(&game);
    d_init();

    while (running) {
        usleep(TICK_RATE);

        if (!pong_tick(&game, read_chars())) {
            running = false;
        }
        d_game(&game, &prev);
        prev = game;
    }

    if (game.scores[0] >= PONG_SCORE_MAX) {
        d_gameover(1);
    } else if (game.scores[1] >= PONG_SCORE_MAX) {
        d_gameover(2);
    }

    ansi_esc_move_cursor(0, PONG_ROWS + 1);
    stdin_set(false);
}
