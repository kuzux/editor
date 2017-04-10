#include <stdlib.h>
#include <string.h>

#include <fled/common.h>
#include <fled/rows.h>

/** 
 * rows_t is essentially a dynamically sized array of
 * row_t elements. It doubles or halves in size if the size 
 * changes enough. The implementation is pretty straightforward as well.
 * In the future, we might make a more efficient data structure
 * maybe with links that only contain the line number and an actual array
 * structure only for the lines actually displayed on screen. However,
 * that might make search or syntax highlighting operations costly.
 */

/**
 * Similarly, row_t is a dynamically sized character array. We can
 * assume that each row is a short-ish one and have less of a performace 
 * issue. The structure actually has two array structures, one for the source  
 * line and one for the rendered line. We might need to make it a rows_t like 
 * structure to support resizing
 * TODO: Separate it to another file
 */

/* Insert a new character. Takes only one character and the idx
 * parameter is a source line index.
 * TODO: Don't do this for every character
 */
void row_insertchar(row_t* row, int at, int c) {
    if(at < 0){
        return;
    }

    if(at > row->len) {
        at = row->len;
    }

    int newlen = row->len+1;
    int newrlen;

    if(c == '\t') {
        newrlen = row->rlen+1;

        while(newrlen % EF->config.tabstop != 0) {
           newrlen++;
        }
    } else {
        newrlen = row->rlen+1;
    }

    int renderat = 0;

    int i;
    for(i = 0; i<at; i++) {
        if(row->buf[i] == '\t'){
            renderat++;
            while(renderat % EF->config.tabstop != 0) {
                renderat++;
            }
        } else {
            renderat++;
        }
    }

    row->buf = realloc(row->buf, newlen);
    row->len++;
    row->rbuf = realloc(row->rbuf, newrlen);
    row->rlen = newrlen;

    memmove(&row->buf[at + 1], &row->buf[at], row->len - at);
    memmove(&row->rbuf[renderat + 1], &row->rbuf[renderat], row->rlen - renderat);

    row->buf[at] = c;
    if(c == '\t') {
        row->rbuf[renderat++] = ' ';

        while(renderat % EF->config.tabstop != 0) {
            row->rbuf[renderat++] = ' ';
        }
    } else {
        row->rbuf[renderat] = c;
    }
}

rows_t* make_rows() {
    rows_t* res;
    res = (rows_t*)malloc(sizeof(rows_t));

    if(res == NULL) {
        DIE("malloc");
    }

    res->rows = malloc(ROWS_INIT_SIZE*sizeof(row_t));
    if(res->rows == NULL) {
        DIE("malloc");
    }

    res->length = 0;
    res->allocated = ROWS_INIT_SIZE;

    return res;
}

void expand_rows(rows_t* rows) {
    if(rows == NULL) {
        return;
    }
    if(rows->rows == NULL) {
        return;
    }

    int newlen = rows->allocated * 2;

    /* A sanity check */
    if(newlen < ROWS_INIT_SIZE) {
        newlen = ROWS_INIT_SIZE;
    }

    rows->rows = realloc(rows->rows, newlen*sizeof(row_t));

    if(rows->rows == NULL) {
        DIE("realloc");
    }
}

void shrink_rows(rows_t* rows) {
    if(rows == NULL) {
        return;
    }
    if(rows->rows == NULL) {
        return;
    }

    int newlen = rows->allocated / 2;

    /* Sanity checks */
    if(rows->length >= newlen) {
        return;
    }

    if(newlen <= ROWS_INIT_SIZE) {
        newlen = ROWS_INIT_SIZE;
    }

    row_t* newrows = (row_t*)malloc(newlen*sizeof(row_t));
    if(newrows == NULL) {
        DIE("malloc");
    }

    memcpy(newrows, rows->rows, rows->length*sizeof(row_t));
    free(rows->rows);
    rows->rows = newrows;
}

void insert_row(rows_t* rows, row_t* row) {
    if(rows->length < rows->allocated) {
        rows->rows[rows->length++] = *row;
    } else {
        expand_rows(rows);
        rows->rows[rows->length++] = *row;
    }
}

void free_rows(rows_t* rows) {
    int i;
    for(i = 0; i< rows->length; i++) {
        free(rows->rows[i].buf);
    }

    free(rows->rows);
    free(rows);
}

void calc_render_row(row_t* row) {
    free(row->rbuf);
    /* TODO: Handle some unicode here */
    int finallen = 0;

    int i = 0, j = 0;

    for(i = 0; i < row->len; i++) {
        if(row->buf[i]=='\t'){
            finallen += FLED_TABSTOP;
        } else {
            finallen++;
        }
    }

    finallen++;

    row->rbuf = malloc(finallen);

    i = 0;
    j = 0;

    while(j < row->len) {
        if(row->buf[j]=='\t') {
            row->rbuf[i++] = ' ';
            while(i % FLED_TABSTOP != 0) {
                row->rbuf[i++] = ' ';
            }
            j++;
        } else {
            row->rbuf[i++] = row->buf[j++];
        }
    }

    row->rbuf[i] = '\0';
    row->rlen = i;
}
