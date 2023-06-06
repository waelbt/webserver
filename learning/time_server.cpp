#include "server.hpp"

// SOCKET create_socket(const char* host, const char *port) {
//     struct addrinfo hints;
//     struct addrinfo *bind_address;
//     SOCKET socket_listen;

//     printf("Configuring local address...\n");
//     memset(&hints, 0, sizeof(hints));
//     hints.ai_family = AF_INET;
//     hints.ai_socktype = SOCK_STREAM;
//     hints.ai_flags = AI_PASSIVE;
//     getaddrinfo(host, port, &hints, &bind_address);

//     printf("Creating socket...\n");    
//     socket_listen = socket(bind_address->ai_family,
//     bind_address->ai_socktype, bind_address->ai_protocol);
//     if (socket_listen < 0) 
//     {
//         fprintf(stderr, "socket() failed. (%s)\n", strerror(errno));
//         exit(1);
//     }

//     printf("Binding socket to local address...\n");
//     if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
//     {
//         fprintf(stderr, "bind() failed. (%s)\n", strerror(errno));
//         exit(1);
//     }
//     freeaddrinfo(bind_address);

//     printf("Listening...\n");
//     if (listen(socket_listen, 10) < 0)
//     {
//         fprintf(stderr, "listen() failed. (%s)\n", strerror(errno));
//         exit(1);
//     }
//     return socket_listen;
// }
// struct in_addr ipAddress;
// inet_pton(AF_INET, "192.168.0.100", &ipAddress)


int main(void)
{
    struct addrinfo hints;
    struct addrinfo *bind_address;
    int socket_listen;
    printf("Configuring local address...\n");

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    // hints.ai_flags = AI_PASSIVE;

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
        fprintf(stderr, "socket() failed. (%s)\n", strerror(errno));
        return 1;
    }
    printf("Binding socket to local address...\n");
    if (bind(socket_listen,
        bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed. (%s)\n", strerror(errno));
        return 1;
    }
    freeaddrinfo(bind_address);
    printf("Listening...\n");
    /* which is 10 in this case, tells listen() how many connections it is allowed to queue up. */
    if (listen(socket_listen, 10) < 0) {
        fprintf(stderr, "listen() failed. (%s)\n", strerror(errno));
        return 1;
    }

    printf("Waiting for connection...\n");
    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);
    int socket_client = accept(socket_listen, (struct sockaddr*) &client_address, &client_len);
    if (socket_client < 0) {
        fprintf(stderr, "accept() failed. (%s)\n", strerror(errno));
        return 1;
    }

    /*This step is completely optional*/
    printf("Client is connected... ");
    char address_buffer[100];
    getnameinfo((struct sockaddr*)&client_address,
    client_len, address_buffer, sizeof(address_buffer), 0, 0,
    NI_NUMERICHOST);
    printf("%s\n", address_buffer);

    /*If nothing has been received yet,recv() blocks until it has something. If the connection is terminated by the client, recv() returns 0 or -1*/
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



    time_t timer;
    time(&timer);
    char *time_msg = ctime(&timer);
    bytes_sent = send(socket_client, time_msg, strlen(time_msg), 0);
     /* send() returns the number of bytes sent. You should generally check that the number of
    bytes sent was as expected, and you should attempt to send the rest if it's not. We are
    ignoring that detail here for simplicity */
    printf("Sent %d of  %d bytes.\n", bytes_sent, (int)strlen(time_msg));

    // By enabling the SO_REUSEADDR option, you prevent the "Address already in use" error message that could occur when attempting to bind to a local address that is still in the TIME_WAIT state. This option is commonly used in server applications that need to quickly restart and rebind to the same address and port.
    // int yes = 1;
    // if (setsockopt(socket_listen, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
    //     perror("setsockopt");
    //     exit(1);
    // } wael reja3 l hadi

    /*We must then close the client connection to indicate to the browser that we've sent all of our data*/
    printf("Closing connection...\n");
    close(socket_client);
    close(socket_listen);
    printf("Finished.\n");
    return 0;
}