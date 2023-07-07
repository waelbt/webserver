#include "../includes/Registry.hpp"

Registry::Registry() : _host(), _port(), _listen_socket()
{}



Registry::Registry(std::string host, std::string port, SOCKET  listen_socket) : _host(host), _port(port), _listen_socket(listen_socket)
{}


Registry::Registry(const Registry& other)
{
	*this = other;
}

Registry& Registry::operator=(const Registry& other)
{
	_host = other._host;
	_port = other._port;
	_listen_socket = other._listen_socket;
	return *this;
}



Registry::~Registry()
{
		// close (_listen_socket);
}