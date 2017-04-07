#include <ctype.h>

#include <fled/common.h>
#include <fled/terminal.h>
#include <fled/input.h>

void scroll(int key) {
    /* Set the current row length if the current row exists */
    int currlen = 0;
    row_t curr;

    if(E->cury + E->offy < E->rows->length) {
        /* Not sure why this is off by one
         * but it seems so
         */
        curr = E->rows->rows[E->cury + E->offy + 1];
        currlen = curr.len;
    }

    switch(key) {

        case ARROW_UP:
            if(E->offy > 0) {
                E->offy--;
            }
            break;
        case ARROW_DOWN:
            if(E->sz_rows + E->offy <= E->rows->length) {
                E->offy++;
            }
            break;
        case ARROW_LEFT:
            if(E->offy > 0) {
                E->offx--;
            }
            break;
        case ARROW_RIGHT:
            DEBUG_LOGF("%d %d %d\n", E->sz_cols, E->offx, currlen);
            DEBUG_LOGF("%d %s\n", E->cury + E->offy, curr.buf);
            if(E->sz_cols + E->offx <= currlen) {
                E->offx++;
            }
            DEBUG_LOG("Do we get there?");
            break;
    }
}

void move_cursor(int key) {
    switch(key) {
        case ARROW_UP:
            if (E->cury > 0) {
                E->cury--;
            } else if (E->cury == 0) {
                scroll(key);
            }
            break;
        case ARROW_DOWN:
            if (E->cury < E->sz_rows - 1) {
                E->cury++;
            } else if (E->cury == E->sz_rows - 1) {
                scroll(key);
            }
            break;
        case ARROW_LEFT:
            if (E->curx > 0) {
                E->curx--;
            } else if (E->curx == 0) {
                scroll(key);
            }
            break;
        case ARROW_RIGHT:
            if (E->curx < E->sz_cols - 1) {
                E->curx++;
            } else if (E->curx == E->sz_cols - 1) {
                DEBUG_LOG("last col");
                scroll(key);
            }
            break;
    }
}

void process_key() {
    int c = readkey();

    int times = 0;

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
            times = E->sz_rows;
            while (times--) {
                /* Use the same up/down movement routine so that
                 * we still have the bounds checking
                 */
                move_cursor(ARROW_UP);
            }
            break;
        case PAGE_DOWN:
            times = E->sz_rows;
            while (times--) {
                move_cursor(ARROW_DOWN);
            }
            break;
        case HOME_KEY:
            E->curx = 0;
            break;
        case END_KEY:
            E->curx = E->sz_cols - 1;
            break;
        case DEL_KEY:
            /* Do nothing */
            break;
        default:
            /* Ignore the keypress */
            break;
    }
}
