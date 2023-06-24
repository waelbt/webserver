/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlimouri <hlimouri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 15:38:53 by waboutzo          #+#    #+#             */
/*   Updated: 2023/06/18 13:09:44 by hlimouri         ###   ########.fr       */
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

void signalHandler(int signal)
{
   if (signal == SIGINT || signal == SIGTSTP)
   {
    	// Webserver::clear_set();
		std::exit(0);
   }
}

int main(int ac, char **av)
{
	std::signal(SIGINT, signalHandler);
	std::signal(SIGPIPE, SIG_IGN);
	try
	{
		std::string content(((ac != 2) ? OpenPath() : OpenPath(av[1])));

		// Webserver::clear_set();
		Webserver webserver(content);
		for (size_t i = 0; i < webserver._registry.size(); i++)
		{
			std::cout << webserver._registry[i]._host << std::endl;
			std::cout << webserver._registry[i]._port << std::endl;
			std::cout << webserver._registry[i]._listen_socket << std::endl;
		}
		// webserver.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << "EXCEPTION : "<< e.what() << std::endl;
	}
	return 0;
}
