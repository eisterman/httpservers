#ifndef H_CHARDYNBUF
#define H_CHARDYNBUF
#include <stdlib.h>
#include <string.h>

typedef struct CharDynBuf {
    char *buffer;  // \x00 at the end NOT GUARANTEED
    size_t capacity;
    size_t length;
} CharDynBuf;

void chardynbuf_init(CharDynBuf * out, size_t capacity);

void chardynbuf_free(CharDynBuf *buf);

void chardynbuf_copyinto(CharDynBuf *dest, char* source, size_t sourcelength);

#endif
