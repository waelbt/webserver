#include "webserve.hpp"

SOCKET create_socket(const char* host, const char *port) {
    struct addrinfo hints;
    struct addrinfo *bind_address;
    SOCKET socket_listen;

    printf("Configuring local address...\n");
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(host, port, &hints, &bind_address);

    printf("Creating socket...\n");    
    socket_listen = socket(bind_address->ai_family,
    bind_address->ai_socktype, bind_address->ai_protocol);
    if (socket_listen < 0) 
    {
        fprintf(stderr, "socket() failed. (%s)\n", strerror(errno));
        exit(1);
    }

    printf("Binding socket to local address...\n");
    if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
    {
        fprintf(stderr, "bind() failed. (%s)\n", strerror(errno));
        exit(1);
    }
    freeaddrinfo(bind_address);

    printf("Listening...\n");
    if (listen(socket_listen, 10) < 0)
    {
        fprintf(stderr, "listen() failed. (%s)\n", strerror(errno));
        exit(1);
    }
    return socket_listen;
}
// struct in_addr ipAddress;
// inet_pton(AF_INET, "192.168.0.100", &ipAddress)