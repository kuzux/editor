#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <assert.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <ncurses.h>

#define DEFAULT_GAP_SIZE 15

#define SUCCESS 0
#define ERR_ARG -1
#define ERR_ALLOC -2
#define ERR_FILEIO -3

/* Represents an unloaded part of the file */
typedef struct {
    /* How do we free a chunk? */
    char* buf;
    size_t len;
} filechunk_t;

/* Each line is a gap buffer */
typedef struct editline {
    /* A line can be unloaded */
    bool loaded;
    filechunk_t chunk;

    /* Allocated size */
    size_t size;
    /* Should we have some kind of pool for buffers <= 100 chars? 
     * as well as editline objects */
    char* buf;

    /* Current length of the string (excluding the gap) */
    size_t currlen;

    struct editline* next;
    struct editline* prev;

    size_t gap_size;

    size_t gap_start;
    size_t gap_end;
} editline_t;

typedef struct {
    editline_t* head;
    size_t num_lines;

    size_t cursor_x;
    size_t cursor_y;

    size_t screen_x;
    size_t screen_y;

    size_t screen_width;
    size_t screen_height;
} editbuffer_t;

static editbuffer_t editbuffer;

int load_file(FILE* fp, filechunk_t* chunk) {
    if(fp == NULL || chunk == NULL) {
        return ERR_ARG;
    }

    /* find the file length */
    fseek(fp, 0, SEEK_END);
    chunk->len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    chunk->buf = (char*)malloc(sizeof(char)*(chunk->len+1));
    if(chunk->buf == NULL) {
        return ERR_ALLOC;
    }

    int read = fread(chunk->buf, 1, chunk->len, fp);
    if(read == chunk->len) {
        return SUCCESS;
    } else {
        return ERR_FILEIO;
    }
}

int get_line_from_chunk(filechunk_t* chunk, editline_t* line) {
    if(chunk == NULL || line == NULL) {
        return ERR_ARG;
    }

    size_t i = 0;
    while(chunk->buf[i]!='\n' && i < chunk->len) {
        i++;
    }

    line->size = i;
    line->currlen = i;
    
    line->buf = malloc(sizeof(char)*(i+1));
    if(line->buf == NULL) {
        return ERR_ALLOC;
    }

    memcpy(line->buf, chunk->buf, i);
    line->buf[i] = 0;
    if(i == chunk->len) {
        /* We're at the end of the file, no new line at the end */
        chunk->buf += i;
        chunk->len -= i;
    } else {
        /* Also ignore the newline at the end */
        chunk->buf += i+1; 
        chunk->len -= i+1;
    }

    line->gap_size = 0;
    line->gap_start = 0;
    line->gap_end = 0;

    /* next and prev pointers are not set here */

    return SUCCESS;
}

int get_lines_for_editbuffer(editbuffer_t* buffer, filechunk_t* chunk) {
    if(chunk == NULL || buffer == NULL) {
        return ERR_ARG;
    }
    size_t num_lines = buffer->screen_height;
    int rc;

    for(size_t i=0; i<num_lines && chunk->len > 0; i++) {
        editline_t* line = (editline_t*)malloc(sizeof(editline_t));
        rc = get_line_from_chunk(chunk, line);
        assert(rc >= 0);
        printw("%s\n", line->buf);
    }

    return SUCCESS;
}

int main(int argc, char** argv) {
    if(argc < 1) {
        printf("usage: %s filename \n", argv[0]);
    }

    int rc;

    filechunk_t filebuf;

    editbuffer.screen_height = 24;

    FILE* fp = fopen(argv[1], "r");
    assert(fp);
    rc = load_file(fp, &filebuf);
    assert(rc >= 0);


    initscr();

    raw(); /* No line buffering */
    keypad(stdscr, true); /* We receive function keys */
    noecho();

    rc = get_lines_for_editbuffer(&editbuffer, &filebuf);
    assert(rc >= 0);


    printw("heya\n");
    printw("press a key\n");
    refresh();

    int ch = getch();
    printw("You pressed: %c (keycode %d)\n", (char)ch, ch);
    printw("Press any key to continue\n");

    getch(); /* Wait for any key on exit */

    endwin();

    return EXIT_SUCCESS;    
}
