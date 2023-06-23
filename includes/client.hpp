#pragma once

#include "configuration.hpp"
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
#include "request.hpp"
#include "response.hpp"


typedef int	SOCKET;
typedef struct addrinfo s_addrinfo;
typedef struct sockaddr_storage s_sockaddr_storage;

struct Client {
	// attributes
	SOCKET _server_socket;
	std::string 										_host;
	std::string 										_port;
	s_sockaddr_storage _address;
	socklen_t _address_length;
	SOCKET _socket;
	Request _request;
	Response _response;
	std::string _data_sent;
	ssize_t _bytesSent;
	bool _remaining; 


	// methods
	Client();
	Client(SOCKET server_socket, std::string host, std::string port);
	Client(const Client& other);
	Client& operator=(const Client& other);
	std::string get_client_address();
	~Client();
};


// s_addrinfo *get_s_addrinfo(std::string host, std::string port);
