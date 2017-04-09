#ifndef _ABUF_H
#define _ABUF_H

/**
 * An append buffer structrure 
 * A super simple structure. Horribly inefficient but shouldn't
 * be longer than a screen long of content
 */

typedef struct abuf {
    char* buf;
    int len;
} abuf_t;

#define ABUF_INIT {NULL, 0}

void abuf_append(abuf_t* ab, const char* s, int len);
void abuf_free(abuf_t* ab);

#endif
