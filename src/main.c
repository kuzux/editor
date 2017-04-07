#include <string.h>
#include <stdio.h>

#include <fled/common.h>
#include <fled/terminal.h>
#include <fled/input.h>
#include <fled/output.h>

/* This program uses the termios stuff and
 * requires a unix-like environment to run
 * linux and OSX are okay, but for windows;
 * see http://viewsourcecode.org/snaptoken/kilo/01.setup.html
 */

/* GLOBAL TODO: handle multibyte encodings
 * or just any non-ASCII one
 * also RTL text etc.
 */

/* A global variable that holds the editor state
 * actually defined (as an extern) in common.h and available to all files
 * that include it
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
    res = get_ws(&E->sz_rows, &E->sz_cols);

    /* If the call fails */
    if(res == -1) {
        DIE("get_ws");
    }

    /* Start at the top left corner */
    E->curx = E->cury = 0;

    /* And with an empty buffer */
    E->rows = make_rows();

    /* Initialize the debug log */
#if DEBUG
    E->debug_log = fopen("debug.log", "w");
    if(E->debug_log == NULL) {
        DIE("debug log");
    }
    DEBUG_LOG("Initialized editor");
#endif
}

void load_file(const char* filename) {
    DEBUG_LOG("Loading file");

    if(E == NULL) {
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
        insert_row(E->rows, &newrow);
    }

    /* Reset the offset */
    E->offx = E->offy = 0;

    free(line);
    fclose(fp);

    DEBUG_LOG("Loaded file");
}

void cleanup() {
    #if DEBUG
    fclose(E->debug_log);
    #endif
}

int main(int argc, char** argv) {
    init_editor();

    if(argc >= 2) {
        load_file(argv[1]);
    }

    rawmode();

    while(1) {
        refresh_screen();
        process_key();
    }

    cleanup();

    return 0;
}
