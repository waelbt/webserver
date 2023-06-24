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
//  _listen_sockets(server_socket(_conf.getHost(), _conf.getPort())) 
{
}


// void Server::setnonblocking(int sock)
// {
// 	int opts;

// 	opts = fcntl(sock,F_GETFL);
// 	(opts < 0) ? throw ServerException("fcntl failed to retrieve the current socket status flags") : (NULL);
// 	opts = (opts | O_NONBLOCK);
// 	(fcntl(sock,F_SETFL,opts) < 0) ? \
// 	throw ServerException("fcntl failed to set set the modified file status flags back to the socket") : (NULL);
// }

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
		std::cout << "drop_client socket  " << _client[i]->_socket << std::endl;
		delete _client[i];
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
	// if (_listen_sockets == 0)
	// {
	// 	std::cout << "xadadawdawd wffawfwaf" << std::endl;
	// 	exit(0);
	// }
 	close(_listen_sockets);
}