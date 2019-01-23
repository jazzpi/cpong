#include "ansi_esc.h"

#include <stdio.h>
#include <stdarg.h>

void ansi_esc_print_csi(const char* seq, ...) {
    va_list ap;
    va_start(ap, seq);

    printf(ANSI_ESC_CSI);
    vprintf(seq, ap);
}

void ansi_esc_clear_screen(void) {
    ansi_esc_print_csi(ANSI_ESC_CSI_ED, ANSI_ESC_CSI_ED_ALL);
    ansi_esc_print_csi(ANSI_ESC_CSI_CUP, 1, 1);
}

void ansi_esc_move_cursor(unsigned col, unsigned row) {
    ansi_esc_print_csi(ANSI_ESC_CSI_CUP, row + 1, col + 1);
}

void ansi_esc_show_cursor(char show) {
    if (show) {
        ansi_esc_print_csi(ANSI_ESC_CSI_SHOW_CURSOR);
    } else {
        ansi_esc_print_csi(ANSI_ESC_CSI_HIDE_CURSOR);
    }
}

void ansi_esc_sgr_reset(void) {
    ansi_esc_print_csi(ANSI_ESC_CSI_SGR_RESET);
}

void ansi_esc_sgr_reverse(void) {
    ansi_esc_print_csi(ANSI_ESC_CSI_SGR_REVERSE);
}

void ansi_esc_fg(char bright, ansi_fg_col fg) {
    if (bright) {
        ansi_esc_print_csi(ANSI_ESC_CSI_FG_BRIGHT, fg);
    } else {
        ansi_esc_print_csi(ANSI_ESC_CSI_FG, fg);
    }
}

void ansi_esc_bg(ansi_bg_col bg) {
    ansi_esc_print_csi(ANSI_ESC_CSI_BG, bg);
}

void ansi_esc_fg_bg(char bright, ansi_fg_col fg, ansi_bg_col bg) {
    if (bright) {
        ansi_esc_print_csi(ANSI_ESC_CSI_FG_BG_BRIGHT, fg, bg);
    } else {
        ansi_esc_print_csi(ANSI_ESC_CSI_FG_BG, fg, bg);
    }
}
