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

fd_set Server::_server_set;
SOCKET Server::_max_socket = 0;
std::vector<Client> Server::_client;

Server::Server(const Configuration& conf):  _conf(conf)
{
	try
	{
		setup_server_socket(_conf.getHost(), _conf.getPort());
		std::cout << _listen_sockets << std::endl;
		FD_SET(_listen_sockets, &_server_set);
		if (_max_socket < _listen_sockets)
			_max_socket = _listen_sockets;
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
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
		if (_client[i].ClientSocket() == client_socket)
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
		FD_SET(_client[size].ClientSocket(), &reads);
		if (_client[size].ClientSocket() > _max_socket)
 			_max_socket = _client[size].ClientSocket();
	}
	if (select(_max_socket+1, &reads, 0, 0, 0) < 0)
		throw CustomeExceptionMsg("select() failed. ("+  std::string(strerror(errno)) + ")");
	return reads;
}

void Server::clear_set()
{
	FD_ZERO(&_server_set);
}


void Server::new_connection_handler(fd_set reads) 
{
	(void) reads;
	//  for (SOCKET fd = 0; fd < _max_server_socket; ++fd)
	// {
        // if (FD_ISSET(fd, &reads))
		// 	_client.insert(_client.end(), Client(fd));
	// }
}



void Server::run()
{
	fd_set reads;

	while (1)
	{
		reads = wait_on_client();
		

	}
}

void Server::showConfig() const
{
	std::cout << _conf << std::endl;
}