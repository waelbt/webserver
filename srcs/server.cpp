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

// fd_set Server::_server_set;
// SOCKET Server::_max_socket = 0;
// std::vector<Client> Server::_client;

Server::Server(const Configuration& conf): _max_socket(), _conf(conf), _client()
{
	setup_server_socket(_conf.getHost(), _conf.getPort());
	FD_ZERO(&_server_set);
	FD_SET(_listen_sockets, &_server_set);
	// FD_SET(0, &_server_set);
	if (_max_socket < _listen_sockets)
		_max_socket = _listen_sockets;
}


s_addrinfo* Server::get_s_addrinfo(std::string host, std::string port)
{
    s_addrinfo hints;
    s_addrinfo *bind_address;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
	// passive flag
    getaddrinfo(host.c_str(), port.c_str(), &hints, &bind_address);
    return bind_address;
}

void Server::setup_server_socket(std::string host, std::string port)
{
	s_addrinfo	*bind_addr(get_s_addrinfo(host, port));

	_listen_sockets = socket(bind_addr->ai_family, bind_addr->ai_socktype, bind_addr->ai_protocol);
	if (_listen_sockets < 0) {
		freeaddrinfo(bind_addr); throw CustomeExceptionMsg("socket() failed. ("+  std::string(strerror(errno)) + ")"); }
	if (bind(_listen_sockets, bind_addr->ai_addr, bind_addr->ai_addrlen)) {
		freeaddrinfo(bind_addr); throw CustomeExceptionMsg("bind() failed. ("+  std::string(strerror(errno)) + ")"); }
	if (listen(_listen_sockets, MAX_PENDING_CNX) < 0) {
		freeaddrinfo(bind_addr); throw CustomeExceptionMsg("listen() failed. ("+  std::string(strerror(errno)) + ")"); }
}

Client& Server::get_client(SOCKET client_socket)
{
	size_t i;

	for(i = 0; i < _client.size(); i++)
	{
		if (_client[i]._socket == client_socket)
			break;
	}
	if(i == _client.size())
		throw CustomeExceptionMsg("client not found");
	return _client[i];
}

fd_set Server::wait_on_client()
{
	fd_set reads = _server_set;
	

	for (size_t size = 0; size < _client.size(); size++)
	{
		FD_SET(_client[size]._socket, &reads);
		if (_client[size]._socket > _max_socket)
 			_max_socket = _client[size]._socket;
	}
	// i should set the timeout parameter 
	if (select(_max_socket + 1, &reads, 0, 0, 0) < 0)
		throw CustomeExceptionMsg("select() failed. ("+  std::string(strerror(errno)) + ")");
	return reads;
}

void Server::drop_client(size_t i)
{
	if (i < _client.size())
	{
		close(_client[i]._socket);
		_client.erase(_client.begin() + i);
	}
}

void Server::run()
{
	fd_set reads;

	while (1)
	{
		reads = wait_on_client();
		if (FD_ISSET(_listen_sockets, &reads))
			_client.insert(_client.end(), Client(_listen_sockets));
		for (size_t i = 0; i < _client.size(); i++)
		{
			if (FD_ISSET(_client[i]._socket, &reads))
			{
				int r = recv(_client[i]._socket, _client[i]._request + _client[i]._received, MAX_REQUEST_SIZE - _client[i]._received, 0);
				if (r < 1)
				{
					std::cout << "Unexpected disconnect from " << _client[i].get_client_address() << std::endl;
					drop_client(i);
				}
				else
				{
					_client[i]._received += r;
					_client[i]._request[_client[i]._received] = '\0';
					std::cout << _client[i]._request << std::endl;
					// request handling
				}
			}
		}
	}
	close(_listen_sockets);
}

void Server::showConfig() const
{
	std::cout << _conf << std::endl;
}