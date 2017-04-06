#include <string.h>

#include <editor/common.h>
#include <editor/output.h>
#include <editor/abuf.h>

void draw_screen(abuf_t* ab) {
    int y;

    /* Define our welcome message */
    char welcome[80];
    int welcomelen = snprintf(welcome, sizeof(welcome),
        "Yet another editor -- version %s", EDITOR_VERSION);

    int wrap = 0;
    int wrapstart = 0;

    /* for each row on the screen */
    for (y = 0; y < E->rows; y++) {
        /* This line clears the current line
         * the last argument is that we are writing 4 bytes
         * and the bytes are:
         * \x1b : the escape byte
         * [ : every escape sequence starts with \x1b[
         * 2 : argument to K, clear the entire line
         * K : clear the current line
         */
        abuf_append(ab, "\x1b[2K", 4);

        /* TODO: Wrapping gets wonky if it takes more than two lines
         * However, wrapping for 2 lines works well.
         * Currently have no idea what causes this
         */
        if (wrap == 1) {
            /* If we are continuing the message from the previous line */
            abuf_append(ab, " ", 1);

            int remlen = welcomelen - wrapstart;
            int writelen = remlen;

            if(remlen > E->cols - 1) {
                /* It still goes on, we have cols-1 characters more to
                 * write on this line.
                 * The bug previously described should be something about 
                 * these lines
                 */

                wrap = 1;
                wrapstart += E->cols - 1;
                writelen = E->cols - 1;
            } else {
                /* We finished writing the line. Don't wrap anymore */
                wrap = 0;

                /* Center the output with some spaces for padding */
                int padding = (E->cols-1-writelen)/2;
                while(padding--) {
                    abuf_append(ab, " ", 1);
                }
            }

            abuf_append(ab, welcome+wrapstart, writelen);
        } else if (y == E->rows / 3) {
            abuf_append(ab, "~", 1);

            int writelen = welcomelen;

            if (welcomelen > E->cols - 1){
                /* The message goes on, we'll need to continue on to the
                 * next line, we were able to write cols - 1 characters
                 * on this line. Set wrap to 1 to continue on the next one
                 * Also set where to be continued on the next line
                 */
                wrap = 1;
                wrapstart = E->cols - 1;
                writelen = E->cols - 1;
            }

            int padding = (E->cols-1-writelen)/2;
            while(padding--) {
                abuf_append(ab, " ", 1);
            }

            abuf_append(ab, welcome, writelen);
        } else {
            abuf_append(ab, "~", 1);
        }

        if (y < E->rows - 1) {
          abuf_append(ab, "\r\n", 2);
        }
    }
}

void refresh_screen() {
    abuf_t ab = ABUF_INIT;

    /* Hide the cursor */
    abuf_append(&ab, "\x1b[?25l", 6);

    /* Position the cursor at top left corner
     * H with no argument = 0;0H (to top left corner, outside of the screen)
     * So that the cursor doesn't effect our repainting
     */
    abuf_append(&ab, "\x1b[H", 3);

    draw_screen(&ab);

    /* Set the cursor to the cursor position defined in the editor config.
     * We also add 1 as to translate between 0-indexed coordinates and
     * 1-indexed coordinates.
     */
    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", E->cury + 1, E->curx + 1);
    abuf_append(&ab, buf, strlen(buf));

    /* Show the cursor again */
    abuf_append(&ab, "\x1b[?25h", 6);
    
    /* write and STDOUT_FILENO are defined in unistd.h */
    write(STDOUT_FILENO, ab.buf, ab.len);

    /* We don't need to free the append buffer since it's defined in the stack
     * but cleaning its internal buffer is generally a good idea.
     */
    free(ab.buf);
}
