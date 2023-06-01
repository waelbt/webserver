#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>

struct my_ifaddrs {
    struct my_ifaddrs  *ifa_next;     /* Pointer to the next structure */
    char            *ifa_name;     /* Interface name */
    unsigned int     ifa_flags;    /* Interface flags */
    struct my_sockaddr *ifa_addr;     /* Interface address */
    struct my_sockaddr *ifa_netmask;  /* Interface netmask */
    struct my_sockaddr *ifa_dstaddr;  /* Interface destination address */
    void            *ifa_data;     /* Interface specific data */
};

struct my_sockaddr {
    sa_family_t sa_family;   /* Address family (e.g., AF_INET for IPv4) */
    char        sa_data[14]; /* Address data */
};

struct my_sockaddr_in {
    sa_family_t    sin_family; /* Address family (AF_INET) */
    in_port_t      sin_port;   /* Port number */
    struct in_addr sin_addr;   /* IPv4 address */
    char           sin_zero[8];/* Padding for structure size */
};

struct my_in_addr {
    in_addr_t s_addr;  /* IPv4 address in network byte order */
};

int main() {
    struct ifaddrs *addresses;

    if (getifaddrs(&addresses) == -1) {
        printf("getifaddrs call failed\n");
    return -1;
    }
    struct ifaddrs *address = addresses;
    while(address) {
        int family = address->ifa_addr->sa_family;
        if (family == AF_INET || family == AF_INET6)
        {
            printf("%s\t", address->ifa_name);
            printf("%s\t", family == AF_INET ? "IPv4" : "IPv6");
            char ap[100];
            getnameinfo(address->ifa_addr, (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6), ap, sizeof(ap), 0, 0, NI_NUMERICHOST);
            printf("\t%s\n", ap);
        }
        address = address->ifa_next;
    }
    freeifaddrs(addresses);
}