#include <ctype.h>

#include <fled/common.h>
#include <fled/terminal.h>
#include <fled/input.h>

/**
 * We are either at the end of a line or past it.
 * If we went past, snap back so that we are at the end of
 * the line
 */
void snap_cursor() {
    if(EF->cury + EF->offy < EF->rows->length) {
        row_t curr = EF->rows->rows[EF->cury+EF->offy];
        if(EF->curx > curr.rlen) { 
            if(curr.rlen < EF->sz_cols) {
                /* We are going back to zero horizontal scroll */
                EF->curx = curr.rlen;
                EF->offx = 0;
            } else if(curr.rlen > EF->offx && curr.rlen < EF->offx + EF->sz_cols) {
                /* We don't need to scroll */
                EF->curx = curr.rlen - EF->offx;
            } else {
                /* We need to scroll */
                EF->curx = EF->sz_cols;
                EF->offx = curr.rlen - EF->sz_cols;
            }
        }
    }
}

/*
 * Assume we are in an edge position.
 * Scroll by one row or column if we need to and
 * able to
 */
void scroll(int key) {
    /* Set the current row length if the current row exists */
    int currlen = 0;
    row_t curr;

    if(EF->cury + EF->offy < EF->rows->length) {
        /**
         * Not sure why this is off by one
         * but it seems so
         */
        curr = EF->rows->rows[EF->cury + EF->offy + 1];
        currlen = curr.rlen;
    }

    switch(key) {
        case ARROW_UP:
            if(EF->offy > 0) {
                EF->offy--;
            }
            break;
        case ARROW_DOWN:
            if(EF->sz_rows + EF->offy <= EF->rows->length) {
                EF->offy++;
            }
            break;
        case ARROW_LEFT:
            if(EF->offx > 0) {
                EF->offx--;

                /* Move right in source coordinates as well */
                EF->srcx--;
            }
            break;
        case ARROW_RIGHT:
            if(EF->sz_cols + EF->offx <= currlen) {
                EF->offx++;

                /* Move right in source coordinates as well */
                EF->srcx++;
            }
            break;
        default:
            DEADCODE;
            break;
    }
}

/**
 * Assume key is an arrow key
 * Move one way accordingly 
 */
void move_cursor(int key) {
    /* Get the length of the current line */
    int currlen = 0;

    if(EF->cury + EF->offy < EF->rows->length) {
        currlen = EF->rows->rows[EF->cury + EF->offy].rlen;
    }

    switch(key) {
        case ARROW_UP:
            if (EF->cury > 0) {
                /* Simply move up */
                EF->cury--;
            } else if (EF->cury == 0) {
                /* We might need to scroll up if we are on the top row */
                scroll(key);
            }
            snap_cursor();
            break;
        case ARROW_DOWN:
            if (EF->cury < EF->sz_rows - 1) {
                EF->cury++;
            } else if (EF->cury == EF->sz_rows - 1) {
                scroll(key);
            }
            snap_cursor();
            break;
        case ARROW_LEFT:
            if (EF->curx > 0) {
                /**
                 * Standard horizontal move
                 * Decrement source coordinates by one 
                 */
                EF->srcx--;
                if(EF->rows->rows[EF->offy+EF->cury].buf[EF->offx+EF->srcx+1]=='\t') {
                    /**
                     * We encountered a tab. Need to move this more 
                     * TODO: This needs to be changed for unicode 
                     */
                    EF->curx--;
                    while (EF->curx % FLED_TABSTOP != 0) {
                        EF->curx--;
                    }
                } else {
                    /* Non-tab character */
                    EF->curx--;
                }
            } else if (EF->curx == 0 && EF->offx != 0) {
                /* We might need to scroll left */
                scroll(key);
            } else if (EF->curx + EF->offx == 0 && EF->cury != 0) {
                /**
                 * We are in the first column, move up to the 
                 * right end of the line 
                 */
                EF->cury--;
                EF->curx = EF->rows->rows[EF->cury + EF->offy].rlen;
                EF->srcx = EF->rows->rows[EF->cury + EF->offy].len;
                snap_cursor();
            }
            break;
        case ARROW_RIGHT:
            /**
             * This is (at keast should be) the same thing as move left, 
             * only flipped
             */
            if (EF->curx < EF->sz_cols - 1 && EF->curx + EF->offx < currlen ) {
                EF->srcx++;

                /**
                 * The last minus one is due to the fact we already 
                 * incremented srcx
                 */
                if(EF->rows->rows[EF->offy+EF->cury].buf[EF->offx+EF->srcx-1]=='\t') {
                    EF->curx++;
                    while (EF->curx % FLED_TABSTOP != 0) {
                        EF->curx++;
                    }
                } else {
                    EF->curx++;
                }

                /**
                 * We might need to scroll horizontally after
                 * going over a tab
                 */
                while(EF->curx >= EF->sz_cols - 1) {
                    scroll(key);
                }
            } else if (EF->curx == EF->sz_cols - 1) {
                scroll(key);
            } else if (EF->curx + EF->offx == currlen && EF->cury > 0) {
                EF->cury++;
                EF->curx = 0;
                EF->srcx = 0;
                snap_cursor();
            }
            break;
        default:
            DEADCODE;
            break;
    }
}

void process_key() {
    int c = readkey();

    int times = 0;

    row_t curr = EF->rows->rows[EF->cury+EF->offy];

    switch(c) {
        case CTRL_KEY('q'):
             /* Continue while the character is not ctrl + q */
            EXIT();
        case ARROW_UP:
        case ARROW_DOWN:
        case ARROW_LEFT:
        case ARROW_RIGHT:
            /* Move the cursor */
            move_cursor(c);
            break;
        case PAGE_UP:
            EF->cury = 0;
            times = EF->sz_rows;
            while (times--) {
                /**
                 * Use the same up/down movement routine so that
                 * we still have the bounds checking
                 */
                move_cursor(ARROW_UP);
            }
            break;
        case PAGE_DOWN:
            EF->cury = EF->sz_rows - 1;
            times = EF->sz_rows;
            while (times--) {
                move_cursor(ARROW_DOWN);
            }
            break;
        case HOME_KEY:
            /* TODO: jump to first non-whitespace character */
            EF->curx = 0;
            EF->srcx = 0;
            break;
        case END_KEY:
            if(curr.rlen < EF->sz_cols) {
                /* We are going back to zero horizontal scroll */
                EF->curx = curr.rlen;
                EF->offx = 0;
            } else if(curr.rlen > EF->offx && curr.rlen < EF->offx + EF->sz_cols) {
                /* We don't need to scroll */
                EF->curx = curr.rlen - EF->offx;
            } else {
                /* We need to scroll */
                EF->curx = EF->sz_cols;
                EF->offx = curr.rlen - EF->sz_cols;
            }
            EF->srcx = curr.len;
            break;
        case DEL_KEY:
            /* Do nothing */
            break;
        default:
            /* Ignore the keypress */
            break;
    }
}
