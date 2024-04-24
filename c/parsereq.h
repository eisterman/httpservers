#ifndef H_PARSEQ
#define H_PARSEQ

#include <sys/types.h>

#include "chardynbuf.h"

typedef struct HTTPReq {
    char method[8]; // \x00 at the end GUARANTEED
    char* url; // \x00 at the end GUARANTEED
    char** headernames; // [(headername, headervalue)]
    char** headervalues;
    size_t n_headers;
    char* body; // \x00 at the end NOT GUARANTEED (it can contain other 0s)
    size_t body_size;
} HTTPReq;

// typedef struct HTTPReq {
//     char method[8]; // \x00 at the end GUARANTEED
//     CharDynBuf url;
//     CharDynBuf *headers; // [(headername, headervalue)]
//     size_t n_headers;
//     CharDynBuf body;
// } HTTPReq;

int parsebuf(HTTPReq* out, char* buf, ssize_t x);

void deinit(HTTPReq* req);

void printreq(HTTPReq* req);

#endif