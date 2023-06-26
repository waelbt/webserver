
#pragma once
#include <string>
#include <iostream>
typedef int	SOCKET;

struct Registry
{
	std::string _host;
	std::string	_port;
	SOCKET  _listen_socket;

	Registry();
	Registry(std::string host, std::string port, SOCKET  listen_socket);
	Registry(const Registry& other);
	Registry& operator=(const Registry& other);
	~Registry();
};