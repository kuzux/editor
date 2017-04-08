#include <ctype.h>

#include <fled/common.h>
#include <fled/terminal.h>
#include <fled/input.h>

void scroll(int key) {
    /* Set the current row length if the current row exists */
    int currlen = 0;
    row_t curr;

    if(EF->cury + EF->offy < EF->rows->length) {
        /* Not sure why this is off by one
         * but it seems so
         */
        curr = EF->rows->rows[EF->cury + EF->offy + 1];
        currlen = curr.len;
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
            }
            break;
        case ARROW_RIGHT:
            DEBUG_LOGF("%d %s\n", EF->cury + EF->offy, curr.buf);
            if(EF->sz_cols + EF->offx <= currlen) {
                EF->offx++;
            }
            DEBUG_LOG("Do we get there?");
            break;
    }
}

void move_cursor(int key) {
    switch(key) {
        case ARROW_UP:
            if (EF->cury > 0) {
                EF->cury--;
            } else if (EF->cury == 0) {
                scroll(key);
            }
            break;
        case ARROW_DOWN:
            if (EF->cury < EF->sz_rows - 1) {
                EF->cury++;
            } else if (EF->cury == EF->sz_rows - 1) {
                scroll(key);
            }
            break;
        case ARROW_LEFT:
            if (EF->curx > 0) {
                EF->curx--;
            } else if (EF->curx == 0) {
                scroll(key);
            }
            break;
        case ARROW_RIGHT:
            if (EF->curx < EF->sz_cols - 1) {
                EF->curx++;
            } else if (EF->curx == EF->sz_cols - 1) {
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
            times = EF->sz_rows;
            while (times--) {
                /* Use the same up/down movement routine so that
                 * we still have the bounds checking
                 */
                move_cursor(ARROW_UP);
            }
            break;
        case PAGE_DOWN:
            times = EF->sz_rows;
            while (times--) {
                move_cursor(ARROW_DOWN);
            }
            break;
        case HOME_KEY:
            EF->curx = 0;
            break;
        case END_KEY:
            EF->curx = EF->sz_cols - 1;
            break;
        case DEL_KEY:
            /* Do nothing */
            break;
        default:
            /* Ignore the keypress */
            break;
    }
}
