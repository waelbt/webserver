
#pragma once
#include <string>
#include <iostream>
typedef int	SOCKET;

struct Registry
{
	std::string _host;
	std::string	_port;
	SOCKET  _listen_socket;

	Registry() : _host(), _port(), _listen_socket()
	{}
	Registry(std::string host, std::string port, SOCKET  listen_socket) : _host(host), _port(port), _listen_socket(listen_socket)
	{}
	Registry(const Registry& other)
	{
		*this = other;
	}
	Registry& operator=(const Registry& other)
	{
		_host = other._host;
		_port = other._port;
		_listen_socket = other._listen_socket;	
		return *this;
	}
	~Registry()
	{
		// close (_listen_socket);
	}
};