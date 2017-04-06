#include <editor/common.h>
#include <editor/terminal.h>
#include <editor/input.h>
#include <editor/output.h>

/* this program uses the termios stuff and
 * requires a unix-like environment to run
 * linux and OSX are okay, but for windows;
 * see http://viewsourcecode.org/snaptoken/kilo/01.setup.html
 */

/* a global variable that holds the editor state
 * actually defined (as an extern) in common.h and available to all files that include it
 * TODO: don't make that a global variable, 
 * is that actually possible when we use atexit?
 */
editor_config_t* E;

void init_editor() {
    int res;

    E = (editor_config_t*)malloc(sizeof(editor_config_t));
    
    /* bail on malloc failure */
    if(E == NULL) {
        DIE("malloc");
    }

    res = get_ws(&E->rows, &E->cols);

    if(res == -1) {
        DIE("get_ws");
    }
}

int main() {
    init_editor();

    rawmode();

    while(1) {
        refresh_screen();
        process_key();
    }

    return 0;
}
