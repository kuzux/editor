#include <string.h>

#include <fled/common.h>
#include <fled/abuf.h>

/**
 * Append a string to our buffer
 * note that this is a very naive implementation and will only
 * work good for small size buffers (like screen buffers)
 * not for e.g. reading large files
 * or doing many append calls for that matter
 */
void abuf_append(abuf_t* ab, const char* s, int len) {
    char* newbuf = realloc(ab->buf, ab->len + len);

    if(newbuf == NULL) {
        return;
    }

    memcpy(&newbuf[ab->len], s, len);
    ab->buf = newbuf;
    ab->len += len;
}

void abuf_free(abuf_t* ab) {
    free(ab->buf);
    free(ab);
}
