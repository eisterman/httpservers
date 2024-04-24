#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "parsereq.h"

#define PORT 8080
#define FPATH(x) "../files/" #x

int main(void) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0) {
        perror("Error initializing server");
        return 1;
    }
    int opt = 1;
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        perror("Error settings options to the socket");
        return 1;
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
    address.sin_port = htons(PORT);
    socklen_t addrlen = sizeof(address);
    if(bind(server_fd, (struct sockaddr*)&address, addrlen) < 0) {
        perror("Error binding the server to address");
        return 1;
    }
    if(listen(server_fd, 3) < 0) {
        perror("Error during socket listen setting");
        return 1;
    }
    while(1) {
        FILE *file = NULL;
        int new_socket;
        if((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) < 0) {
            fprintf(stderr, "Error accepting new socket");
            continue;
        }
        char *buffer = malloc(1024);
        char *writer = buffer;
        uint64_t bufsize = 1024;
        ssize_t valread;
        // Read the request (in fact, the final version probably will use a static buffer and run what it need.)
        // This code will be useful for managing longer-than-1024 byte things like the Optional Body.
        do {
            valread = read(new_socket, writer, bufsize-(writer-buffer));
            writer += valread;
            printf("DEBUG: %ld %ld - %ld\n", bufsize, (writer-buffer), valread);
            if(bufsize == (writer-buffer)) {
                buffer = realloc(buffer, bufsize+1024);
                writer = buffer+bufsize;
                bufsize += 1024;
            } else {
                break;
            }
        } while (1);
        // Add the final null character, you are guaranteed to have the space to do so
        *writer = 0;
        writer++;
        // Print the buffer
        printf("%ld: %s\nEOF\n", valread, buffer);
        // Parse request
        HTTPReq req = {0};
        int parserr;
        if ((parserr = parsebuf(&req, buffer, valread)) < 0) {
            fprintf(stderr, "Error parsing request: %d", parserr);
            goto terminate;
        }
        // TODO: TODO
        printreq(&req);
        // Render Test Response
        char* filename = FPATH(index.html);
        file = fopen(filename, "r");
        if(file == NULL) {
            fprintf(stderr, "Error opening file %s", filename);
            goto terminate;
        }
        char* cmdline = "HTTP/1.1 200 OK\r\n";
        send(new_socket, cmdline, strlen(cmdline), 0);
        fseek(file, 0, SEEK_END);
        uint64_t filelength = ftell(file);
        fseek(file, 0, SEEK_SET);
        char contentlength[64] = "Content-Length: ";
        snprintf(contentlength+strlen(contentlength), 64-strlen(contentlength)-1, "%ld\r\n", filelength);
        send(new_socket, contentlength, strlen(contentlength), 0);
        send(new_socket, "\r\n", 2, 0);
        char outbuffer[1024] = { 0 };
        ssize_t freaded = 0;
        while((freaded = fread(outbuffer, 1, 1024, file)) > 0) {
            send(new_socket, outbuffer, freaded, 0);
        }
        printf("Index sent\n");
        terminate:
        if(file != NULL) fclose(file);
        close(new_socket);
        free(buffer);
    }
    close(server_fd);
    return 0;
}

