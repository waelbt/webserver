#include "../includes/server.hpp"

s_addrinfo *get_s_addrinfo(const std::string& host, const std::string& port)
{
    s_addrinfo hints;
    s_addrinfo *bind_address;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(host.c_str(), port.c_str(), &hints, &bind_address);
    return bind_address;
}

SOCKET create_socket(const std::string& host, const std::string& port)
{
	SOCKET		socket_listen;
	s_addrinfo	*bind_addr(get_s_addrinfo(host, port));

	socket_listen = socket(bind_addr->ai_family, bind_addr->ai_socktype, bind_addr->ai_protocol);
	freeaddrinfo(bind_addr);
	if (socket_listen < 0) 
		throw CustomeExceptionMsg("socket() failed. ("+  std::string(strerror(errno)) + ")");
	if (bind(socket_listen, bind_addr->ai_addr, bind_addr->ai_addrlen))
		throw CustomeExceptionMsg("bind() failed. ("+  std::string(strerror(errno)) + ")");
	if (listen(socket_listen, MAX_PENDING_CNX) < 0)
		throw CustomeExceptionMsg("listen() failed. ("+  std::string(strerror(errno)) + ")");
	return socket_listen;
}