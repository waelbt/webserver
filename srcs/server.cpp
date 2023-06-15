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


void Server::setnonblocking(int sock)
{
	int opts;

	opts = fcntl(sock,F_GETFL);
	(opts < 0) ? throw ServerException("fcntl failed to retrieve the current socket status flags") : (NULL);
	opts = (opts | O_NONBLOCK);
	(fcntl(sock,F_SETFL,opts) < 0) ? \
	throw ServerException("fcntl failed to set set the modified file status flags back to the socket") : (NULL);
}

void Server::setup_server_socket(std::string host, std::string port)
{	
	int yes;
	s_addrinfo hints;
    s_addrinfo *bind_addr;
	std::string error_message("getaddrinfo system call failed.");

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(host.c_str(), port.c_str(), &hints, &bind_addr);
	if (bind_addr)
	{
		_listen_sockets = socket(bind_addr->ai_family, bind_addr->ai_socktype, bind_addr->ai_protocol);
		Webserver::add_socket(_listen_sockets);
		if (_listen_sockets < 0)
			error_message = "socket system call failed.";
		setsockopt(_listen_sockets, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
		setnonblocking(_listen_sockets);
		if (bind(_listen_sockets, bind_addr->ai_addr, bind_addr->ai_addrlen))
			error_message = "bind system call failed.";
		if (listen(_listen_sockets, MAX_PENDING_CNX) < 0)
			error_message = "listen system call failed.";
		freeaddrinfo(bind_addr);
		return ;
	}
	freeaddrinfo(bind_addr);
	throw ServerException(error_message);
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

std::vector<Client *>& Server::get_clients()
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
		close(_client[i]->_socket);
		_client.erase(_client.begin() + i);
	}
}

void Server::clear_clients()
{
	for (size_t i = 0; i < _client.size(); i++)
		delete _client[i];
}


Server::ServerException::ServerException() : CustomeExceptionMsg()
{}

Server::ServerException::ServerException(const std::string& message) : CustomeExceptionMsg(message)
{}

Server::ServerException::~ServerException() throw()
{}



void Server::showConfig() const
{
	std::cout << _conf << std::endl;
}

Server::~Server()
{
	clear_clients();
 	close(_listen_sockets);
}