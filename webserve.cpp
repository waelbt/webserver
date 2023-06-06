#include "webserve.hpp"
#include <iostream>

// TO DO CREATE A SIMPLE WEBSERVER (SOCKET IN THE BLOCK MODE)

int main(int argc, char *argv[])
{
    struct addrinfo hints;
    struct addrinfo *bind_address;
    int socket_listen;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo("10.12.9.8", "8080", &hints, &bind_address);

    socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
    if (socket_listen < 0) {
        fprintf(stderr, "socket() failed. (%s)\n", strerror(errno));
        return 1;
    }

     if (bind(socket_listen,
        bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed. (%s)\n", strerror(errno));
        return 1;
    }
    freeaddrinfo(bind_address);

    if (listen(socket_listen, 10) < 0) {
        fprintf(stderr, "listen() failed. (%s)\n", strerror(errno));
        return 1;
    }

    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);
    int socket_client = accept(socket_listen, (struct sockaddr*) &client_address, &client_len);
    printf("Client is connected... ");
    char address_buffer[100];
    getnameinfo((struct sockaddr*)&client_address,
    client_len, address_buffer, sizeof(address_buffer), 0, 0,
    NI_NUMERICHOST);
    printf("%s\n", address_buffer);

    printf("Reading request...\n");
    char request[1024];
    int bytes_received = recv(socket_client, request, 1024, 0);
    if (bytes_received < 0){
        fprintf(stderr, "accept() failed. (%s)\n", strerror(errno));
        return 1;
    }
    printf("Received %d bytes.\n", bytes_received);
    /*It is a common mistake to try
    printing data that's received from recv() directly as a C string. There is no guarantee that
    the data received from recv() is null terminated! */
    printf("%.*s", bytes_received, request);
    printf("Sending response...\n");
    const char *response = "HTTP/1.1 200 OK\r\n" "Connection: close\r\n" "Content-Type: text/plain\r\n\r\n" "Local time is: ";
    int bytes_sent = send(socket_client, response, strlen(response), 0);
    printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(response));
    return 0;
}