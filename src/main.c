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
    res = get_ws(&E->sz_rows, &E->sz_cols);

    /* If the call fails */
    if(res == -1) {
        DIE("get_ws");
    }

    /* Start at the top left corner */
    E->curx = E->cury = 0;

    /* And with an empty buffer */
    E->num_rows = 0;
}

void load_file(const char* filename) {
    FILE* fp = fopen(filename, "r");

    if(fp == NULL) {
        DIE("fopen");
    }

    E->rows = (row_t*)malloc(1*sizeof(row_t));
    if(E->rows == NULL) {
        DIE("malloc");
    }
        
    size_t linecap = 0;
    int linelen;
    char* line = NULL;

    linelen = getline(&line, &linecap, fp);

    if(linelen != -1) {
        if(linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r')) {
            linelen--;
        }

        E->rows[0].len = linelen;
        E->rows[0].buf = malloc(linelen + 1);
        if(E->rows[0].buf == NULL) {
            DIE("malloc");
        }

        memcpy(E->rows[0].buf, line, linelen);
        E->rows[0].buf[E->rows[0].len] = '\0';

        E->num_rows = 1;
    }

    free(line);
    fclose(fp);
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

    return 0;
}
