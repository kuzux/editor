#ifndef _ROWS_H
#define _ROWS_H

typedef struct row {
    /* TODO: Do we need a faster implementation
     * or one that uses less memory?
     * Is it a form of premature optimization?
     */

    /* Exact contents */
    char* buf;
    int len;

    /* Rendered output */
    char* rbuf;
    int rlen;
} row_t;

/* That is a simple vector structure for now, but in the future,
 * we'll probably need to implement a better (and more efficient data
 * structure that will speed things up
 */

#define ROWS_INIT_SIZE 128

typedef struct rows {
    row_t* rows;
    int length;
    int allocated;
} rows_t;

rows_t* make_rows();
void insert_row(rows_t* rows, row_t* row);
void free_rows(rows_t* rows);

void calc_render_row(row_t* row);

#endif
