#ifndef __LIB_ANSI_ESC_H_
#define __LIB_ANSI_ESC_H_

#include <limits.h>

/**
 * This file contains definitions for ANSI escape sequences.
 *
 * Anything returned to userspace (from io_read()) should either fit in an
 * unsigned char or be in ansi_esc_seq, * and it should not be
 * ANSI_ESC_SEQ_NOTINSEQ or ANSI_ESC_SEQ_NOTFINISHED (i.e. 0 or -1).
 */

#define ANSI_ESC_NOTFINISHED -1

typedef enum {
    ANSI_ESC_SEQ_NOTINSEQ = 0,
    ANSI_ESC_SEQ_CSI_CUU = UCHAR_MAX + 1,
    ANSI_ESC_SEQ_CSI_CUD,
    ANSI_ESC_SEQ_CSI_CUB,
    ANSI_ESC_SEQ_CSI_CUF,
    ANSI_ESC_SEQ_NOTFINISHED = ANSI_ESC_NOTFINISHED
} ansi_esc_seq;

#define ANSI_ESC_CSI_CHAR '['
#define ANSI_ESC_ESC "\033"
#define ANSI_ESC_CSI ANSI_ESC_ESC "["
#define ANSI_ESC_CSI_ED "%uJ"
#define ANSI_ESC_CSI_ED_TO_END 0
#define ANSI_ESC_CSI_ED_TO_BEG 1
#define ANSI_ESC_CSI_ED_ALL 2
#define ANSI_ESC_CSI_ED_ALL_AND_SCROLLBACK 3
#define ANSI_ESC_CSI_CUP "%u;%uH"
#define ANSI_ESC_CSI_SHOW_CURSOR "?25h"
#define ANSI_ESC_CSI_HIDE_CURSOR "?25l"
#define ANSI_ESC_CSI_FG "%um"
#define ANSI_ESC_CSI_FG_BRIGHT "1;%um"
#define ANSI_ESC_CSI_BG "%um"
#define ANSI_ESC_CSI_FG_BG "%u;%um"
#define ANSI_ESC_CSI_FG_BG_BRIGHT "1;%u;%um"
#define ANSI_ESC_CSI_SGR_RESET "0m"
#define ANSI_ESC_CSI_SGR_REVERSE "7m"

__attribute__ ((format(printf, 1, 2)))
void ansi_esc_print_csi(const char* seq, ...);

/**
 * Clear the screen and move cursor to top left.
 */
void ansi_esc_clear_screen(void);

/**
 * Move the cursor to the zero-based coordinates.
 */
void ansi_esc_move_cursor(unsigned row, unsigned col);

/**
 * Hide/show the cursor.
 */
void ansi_esc_show_cursor(char show);

typedef enum {
    FG_BLACK = 30,
    FG_RED,
    FG_GREEN,
    FG_YELLOW,
    FG_BLUE,
    FG_MAGENTA,
    FG_CYAN,
    FG_WHITE,
} ansi_fg_col;

typedef enum {
    BG_BLACK = 40,
    BG_RED,
    BG_GREEN,
    BG_YELLOW,
    BG_BLUE,
    BG_MAGENTA,
    BG_CYAN,
    BG_WHITE,
} ansi_bg_col;

/**
 * Reset all ANSI SGR attributes.
 */
void ansi_esc_sgr_reset(void);
/**
 * Reverse video. Undo with ansi_esc_sgr_reset()
 */
void ansi_esc_sgr_reverse(void);
/**
 * Set a foreground color.
 */
void ansi_esc_fg(char bright, ansi_fg_col fg);
/**
 * Set a background color.
 */
void ansi_esc_bg(ansi_bg_col bg);
/**
 * Set a fore- and background color.
 */
void ansi_esc_fg_bg(char bright, ansi_fg_col fg, ansi_bg_col bg);

#endif /* __LIB_ANSI_ESC_H_ */
