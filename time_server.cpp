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
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)


int main(void)
{
    struct addrinfo hints;
    struct addrinfo *bind_address;
    SOCKET socket_listen;
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
    return 0;
}