#include <ctype.h>

#include <editor/common.h>
#include <editor/terminal.h>
#include <editor/input.h>

void move_cursor(int key) {
    switch(key) {
        case ARROW_UP:
            if (E->cury > 0) {
                E->cury--;
            }
            break;
        case ARROW_DOWN:
            if (E->cury < E->rows - 1) {
                E->cury++;
            }
            break;
        case ARROW_LEFT:
            if (E->curx > 0) {
                E->curx--;
            }
            break;
        case ARROW_RIGHT:
            if (E->curx < E->cols - 1) {
                E->curx++;
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
            times = E->rows;
            while (times--) {
                /* Use the same up/down movement routine so that
                 * we still have the bounds checking
                 */
                move_cursor(ARROW_UP);
            }
            break;
        case PAGE_DOWN:
            times = E->rows;
            while (times--) {
                move_cursor(ARROW_DOWN);
            }
            break;
        case HOME_KEY:
            E->curx = 0;
            break;
        case END_KEY:
            E->curx = E->cols - 1;
            break;
        case DEL_KEY:
            /* Do nothing */
            break;
        default:
            /* Ignore the keypress */
            break;
    }
}
