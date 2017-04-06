#ifndef _ABUF_H
#define _ABUF_H

/* append buffer structrure 
 * TODO: replace this with something a bit more performant */

typedef struct abuf {
    char* buf;
    int len;
} abuf_t;

#define ABUF_INIT {NULL, 0}

void abuf_append(abuf_t* ab, const char* s, int len);
void abuf_free(abuf_t* ab);

#endif
