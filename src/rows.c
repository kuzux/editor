#include <stdlib.h>
#include <string.h>

#include <fled/common.h>
#include <fled/rows.h>

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
    row->rbuf = malloc(row->len + 1);

    int i = 0, j = 0;

    while(j < row->len) {
        row->rbuf[i++] = row->buf[j++];
    }
    row->rbuf[i] = '\0';
    row->rlen = i;
}
