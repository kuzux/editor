#include <editor/common.h>
#include <editor/output.h>
#include <editor/abuf.h>

void draw_screen(abuf_t* ab) {
  int y;
  for (y = 0; y < E->rows; y++) {
    /* this line clears the current line
     * the last argument is that we are writing 4 bytes
     * and the bytes are:
     * \x1b : the escape byte
     * [ : every escape sequence starts with \x1b[
     * K : clear the current line
     */
    abuf_append(ab, "\x1b[K", 3);

    abuf_append(ab, "~", 1);

    if (y < E->rows - 1) {
      abuf_append(ab, "\r\n", 2);
    }
  }
}

void refresh_screen() {
    abuf_t ab = ABUF_INIT;

    /* hide the cursor */
    abuf_append(&ab, "\x1b[?25l", 6);
    abuf_append(&ab, "\x1b[H", 3);

    draw_screen(&ab);
    
    /* position the cursor at top left corner
     * H with no argument = 0;0H (to top left corner)
     */
    abuf_append(&ab, "\x1b[H", 3);
    /* show the cursor again */
    abuf_append(&ab, "\x1b[?25h", 6);
    
    /* write and STDOUT_FILENO are defined in unistd.h */
    write(STDOUT_FILENO, ab.buf, ab.len);

    /* we don't need to free the append buffer since it's defined in the stack
     * but cleaning its internal buffer is a good idea
     */
    free(ab.buf);
}
