#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#include <fled/common.h>
#include <fled/terminal.h>
#include <fled/input.h>
#include <fled/output.h>

/**
 * This program uses the termios stuff and
 * requires a unix-like environment to run
 * linux and OSX are okay, but for windows;
 * see http://viewsourcecode.org/snaptoken/kilo/01.setup.html
 */

/**
 * GLOBAL TODO: handle multibyte encodings
 * or just any non-ASCII one
 * also RTL text etc.
 */

/**
 * A global variable that holds the editor state
 * actually defined (as an extern) in common.h and available to all files
 * that include it
 * The name is a joke. It used to be called just E
 * TODO: don't make that a global variable, 
 * is that actually possible when we use atexit?
 * Or at least, make it so that only 
 * the truly global stuff need to be there
 */
fled_state_t* EF;

void resize_editor(int signo) {
    if(signo!=SIGWINCH) {
        DIE("resize_editor");
    }

    int res = get_ws(&EF->sz_rows, &EF->sz_cols);
    DEBUG_LOG("resized\n");

    if(res == -1) {
        DIE("get_ws");
    }
}

void init_editor() {
    /* Resize the editor each time our terminal size changes */
    signal(SIGWINCH, resize_editor);

    int res;

    EF = (fled_state_t*)malloc(sizeof(fled_state_t));

    /* Bail on malloc failure */
    if(EF == NULL) {
        DIE("malloc");
    }

    /* Get the window size */
    res = get_ws(&EF->sz_rows, &EF->sz_cols);

    /* If the call fails */
    if(res == -1) {
        DIE("get_ws");
    }

    /* Start at the top left corner */
    EF->curx = EF->cury = 0;
    EF->srcx = 0;

    /* Start at the top left corner */
    EF->offx = EF->offy = 0;

    /* And with an empty buffer */
    EF->rows = make_rows();

    /* Initialize the debug log */
#if DEBUG
    EF->debug_log = fopen("debug.log", "w");
    if(EF->debug_log == NULL) {
        DIE("debug log");
    }
    DEBUG_LOG("Initialized editor");
#endif
}

void load_file(const char* filename) {
    DEBUG_LOG("Loading file");

    if(EF == NULL) {
        DIE("editor config");
    }

    FILE* fp = fopen(filename, "r");

    if(fp == NULL) {
        DIE("fopen");
    }

    size_t linecap = 0;
    int linelen;
    char* line = NULL;

    while((linelen = getline(&line, &linecap, fp)) != -1) {
        /* TODO: handle unicode */
        if(linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r')) {
            linelen--;
        }

        row_t newrow;

        newrow.len = linelen;
        newrow.buf = malloc(linelen + 1);
        if(newrow.buf == NULL) {
            DIE("malloc");
        }

        memcpy(newrow.buf, line, linelen);
        newrow.buf[linelen] = '\0';

        newrow.rbuf = NULL;
        newrow.rlen = 0;

        calc_render_row(&newrow);

        insert_row(EF->rows, &newrow);
    }

    /* Reset the offset */
    EF->offx = EF->offy = 0;

    free(line);
    fclose(fp);

    DEBUG_LOG("Loaded file");
}

void cleanup() {
#if DEBUG
    fclose(EF->debug_log);
#endif
}

int main(int argc, char** argv) {
    init_editor();

    /* Load a file if we are given an argument */
    if(argc >= 2) {
        load_file(argv[1]);
    }

    /**
     * Enter raw mode, and set up a callback to exit at the end
     * this function is defined in terminal.c
     */
    rawmode();

    /* for each frame */
    while(1) {
        refresh_screen();
        process_key();
    }

    cleanup();
    DEBUG_LOG("Exiting normally");

    return 0;
}
