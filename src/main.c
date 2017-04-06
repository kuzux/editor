#include <fled/common.h>
#include <fled/terminal.h>
#include <fled/input.h>
#include <fled/output.h>

/* This program uses the termios stuff and
 * requires a unix-like environment to run
 * linux and OSX are okay, but for windows;
 * see http://viewsourcecode.org/snaptoken/kilo/01.setup.html
 */

/* A global variable that holds the editor state
 * actually defined (as an extern) in common.h and available to all files that include it
 * TODO: don't make that a global variable, 
 * is that actually possible when we use atexit?
 */
fled_config_t* E;

void init_editor() {
    int res;

    E = (fled_config_t*)malloc(sizeof(fled_config_t));

    /* Bail on malloc failure */
    if(E == NULL) {
        DIE("malloc");
    }

    /* Get the window size */
    res = get_ws(&E->rows, &E->cols);

    /* If the call fails */
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
