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

Server::Server(const Configuration& conf):  _conf(conf)
{
	try
	{
		_listen_sockets = create_socket(_conf.getHost(), _conf.getPort());
	}
	catch(std::exception& e)
	{
		// std::cerr << "server  number" + std::to_string(this->_id) +  ": " << e.what() << std::endl;
	}
}

void Server::run()
{
	
}

// void Server::setup()
// {
// }

void Server::showConfig() const
{
	std::cout << _conf << std::endl;
}