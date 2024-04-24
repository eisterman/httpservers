#include "chardynbuf.h"

void chardynbuf_init(CharDynBuf * out, size_t capacity) {
    out->buffer = (char *)malloc(capacity);
    out->capacity = capacity;
    out->length = 0;
}

void chardynbuf_free(CharDynBuf *buf) {
    free(buf->buffer);
}

void chardynbuf_copyinto(CharDynBuf *dest, char* source, size_t sourcelength) {
    if(dest->capacity < sourcelength) {
        dest->buffer = realloc(dest->buffer, sourcelength);
        dest->capacity = sourcelength;
    }
    memcpy(dest->buffer, source, sourcelength);
    dest->length = sourcelength;
}
