//lib
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

//macros

int main(void)
{
    struct addrinfo hints;
    struct addrinfo *bind_address;
    int socket_listen;
    printf("Configuring local address...\n");

    memset(&hints, 0, sizeof(hints)); //remove garbage value
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(0, "8080", &hints, &bind_address);
    /* he getaddrinfo() function has many uses, but for our
    purpose, it generates an address that's suitable for bind(). To make it generate this, we
    must pass in the first parameter as NULL and have the AI_PASSIVE flag set in
    hints.ai_flags. */

    /*only privileged users on Unix-like operating systems can bind to ports 0 through 1023, so we will use 8080*/

    /* only one program can bind to a particular port at a time. If you try to use a port that is already in use, then the call to bind() fails*/

    printf("Creating socket...\n");
    socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
    if (socket_listen < 0) {
        fprintf(stderr, "socket() failed. (%d)\n", errno);
        return 1;
    }
    printf("Binding socket to local address...\n");
    if (bind(socket_listen,
        bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed. (%d)\n", errno);
        return 1;
    }
    freeaddrinfo(bind_address);
    printf("Listening...\n");
    /* which is 10 in this case, tells listen() how many connections it is allowed to queue up. */
    if (listen(socket_listen, 10) < 0) {
        fprintf(stderr, "listen() failed. (%d)\n", errno);
        return 1;
    }

    printf("Waiting for connection...\n");
    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);
    int socket_client = accept(socket_listen, (struct sockaddr*) &client_address, &client_len);
    if (socket_client < 0) {
        fprintf(stderr, "accept() failed. (%d)\n", errno);
        return 1;
    }

    /*This step is completely optional*/
    printf("Client is connected... ");
    char address_buffer[100];
    getnameinfo((struct sockaddr*)&client_address,
    client_len, address_buffer, sizeof(address_buffer), 0, 0,
    NI_NUMERICHOST);
    printf("%s\n", address_buffer);
    return 0;
}