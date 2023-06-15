/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: waboutzo <waboutzo@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 15:38:53 by waboutzo          #+#    #+#             */
/*   Updated: 2023/06/15 15:46:38 by waboutzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/server.hpp"

#include <iostream>
#include <csignal>

std::string OpenPath(const std::string& file_name = DefaultPath)
{
	std::string content;
	std::ifstream file(file_name.c_str());

	if (!file.is_open())
		throw CustomeExceptionMsg(FileFailed + file_name);
	std::getline(file, content, '\0');
	file.close();
	return content;
}

//void signalHandler(int signal)
//{
//    if (signal == SIGINT)
//    {
//        for (SOCKET fd = 0; fd <=  Webserver::_max_socket; fd++) {
//			if (FD_ISSET(fd, &Webserver::_socketset) || FD_ISSET(fd, &Webserver::writes))
//				close(fd);
//		}
//        std::exit(0);
//    }
//}

int main(int ac, char **av)
{
	try
	{
		std::string content(((ac != 2) ? OpenPath() : OpenPath(av[1])));
		//std::signal(SIGINT, signalHandler);

		Webserver::clear_set();
		Webserver webserver(content);

		webserver.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << "EXCEPTION : "<< e.what() << std::endl;
	}
	return 0;
}
