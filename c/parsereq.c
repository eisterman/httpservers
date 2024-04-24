#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "parsereq.h"
#include "chardynbuf.h"

int parsebuf(HTTPReq* out, char* buf, ssize_t x) {
    char *headername = NULL, *headervalue = NULL;
    // Prepare for first line parsing
    char *p0 = buf, *p = strstr(buf, "\r\n");
    if(p == NULL) goto error;
    out->url = calloc(1024, sizeof(char));
    // Parse the first line in a separated buffer
    // There's no real reason, it's just so we can use sscanf for this one!
    char httphead[1024] = {0};
    if(p-p0 >= 1024) goto error;  // Verify if first line stay in the buffer
    memcpy(httphead, p0, p-p0);  // already nullterminated if dont copy 1024.
    int parsed = sscanf(httphead, "%7s %1023s HTTP/1.1", out->method, out->url);
    if(parsed != 2) goto error;
    p0 = p+2;
    // Alloc header arrays
    out->headernames = calloc(256, sizeof(char*));
    out->headervalues = calloc(256, sizeof(char*));
    // Header parsing loop
    while(1) {
        p = strstr(p0, "\r\n");
        if(p == NULL) goto error; // No \r\n!
        // If line contains only CRLF so it's the limit between headers and body
        if(p == p0) {
            p0 += 2;
            break;
        }
        headername = calloc(1024, sizeof(char));
        headervalue = calloc(4096, sizeof(char));
        // Split at first ": ". Before is header,
        p = strstr(p0, ": ");
        if(p-p0 >= 1024) goto error;
        memcpy(headername, p0, p-p0);
        headername[p-p0] = 0;
        p0 = p+2;
        // after and to CRLF is value.
        p = strstr(p0, "\r\n");
        if(p-p0 >= 4096) goto error;
        memcpy(headervalue, p0, p-p0);
        headervalue[p-p0] = 0;
        p0 = p+2;
        // Commit
        out->headernames[out->n_headers] = headername;
        out->headervalues[out->n_headers] = headervalue;
        out->n_headers++;
    }
    // Body
    size_t body_size = x-(p0-buf);
    char* body = malloc(body_size);
    memcpy(body, p0, body_size);
    strncpy(body, p0, body_size);
    out->body = body;
    out->body_size = body_size;
    return 0;

    error:
    free(headername);
    free(headervalue);
    deinit(out);
    return -1;
}

void deinit(HTTPReq* req) {
    free(req->body);
    free(req->url);
    for(int i = 0; i < req->n_headers; i++) {
        free(req->headernames[i]);
        free(req->headervalues[i]);
    }
    free(req->headernames);
    free(req->headervalues);
}

void printreq(HTTPReq* req) {
    printf("%s %s HTTP/1.1\n", req->method, req->url);
    for(int i = 0; i < req->n_headers; i++) {
        printf("%s: %s\n", req->headernames[i], req->headervalues[i]);
    }
    printf("\n");
    for(int i = 0; i < req->body_size; i++) {
        char c = req->body[i];
        if(isprint(c)) printf("%c", c);
        else printf("\\x%02x", c);
    }
}