/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: waboutzo <waboutzo@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 15:38:53 by waboutzo          #+#    #+#             */
/*   Updated: 2023/05/27 16:58:49 by waboutzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/server.hpp"

Server::Server() : _conf(), _listen_sockets(), _client()
{
}

Server::Server(const Configuration& conf): _conf(conf), _client()
{
	setup_server_socket(_conf.getHost(), _conf.getPort());

}


// s_addrinfo* Server::get_s_addrinfo(std::string host, std::string port)
// {
//     s_addrinfo hints;
//     s_addrinfo *bind_address;

//     memset(&hints, 0, sizeof(hints));
//     hints.ai_family = AF_INET;
//     hints.ai_socktype = SOCK_STREAM;
// 	// passive flag
//     getaddrinfo(host.c_str(), port.c_str(), &hints, &bind_address);
//     return bind_address;
// }

void Server::setup_server_socket(std::string host, std::string port)
{	
	s_addrinfo hints;
    s_addrinfo *bind_addr;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(host.c_str(), port.c_str(), &hints, &bind_addr);

	_listen_sockets = socket(bind_addr->ai_family, bind_addr->ai_socktype, bind_addr->ai_protocol);
	if (_listen_sockets < 0) {
		freeaddrinfo(bind_addr); throw CustomeExceptionMsg("socket() failed. ("+  std::string(strerror(errno)) + ")"); }
	if (bind(_listen_sockets, bind_addr->ai_addr, bind_addr->ai_addrlen)) {
		freeaddrinfo(bind_addr); throw CustomeExceptionMsg("bind() failed. ("+  std::string(strerror(errno)) + ")"); }
	if (listen(_listen_sockets, MAX_PENDING_CNX) < 0) {
		freeaddrinfo(bind_addr); throw CustomeExceptionMsg("listen() failed. ("+  std::string(strerror(errno)) + ")"); }
	freeaddrinfo(bind_addr);
}


Server::Server(const Server& other)
{
	*this = other;
}

Server& Server::operator=(const Server& other)
{
	this->_conf =  other._conf;
	this->_listen_sockets =  other._listen_sockets;
	this->_client =  other._client;
	return *this;
}

SOCKET Server::get_listen_sockets() const
{
	return _listen_sockets;
}

std::vector<Client>& Server::get_clients()
{
	return _client;
}

Configuration Server::get_configuration() const
{
	return _conf;
}

void Server::drop_client(size_t i)
{
	if (i < _client.size())
	{
		close(_client[i]._socket);
		_client.erase(_client.begin() + i);
	}
}

void Server::showConfig() const
{
	std::cout << _conf << std::endl;
}

Server::~Server()
{
 	// close(_listen_sockets);
}