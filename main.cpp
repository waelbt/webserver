/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: waboutzo <waboutzo@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 15:38:53 by waboutzo          #+#    #+#             */
/*   Updated: 2023/06/26 02:50:54 by waboutzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/webserver.hpp"

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
    	Webserver::clear_set();
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

		Webserver::clear_set();
		Webserver webserver(content);
		const std::vector<Registry>& registry = webserver.get_registry();
		if (!registry.empty())
		{
			std::cout << "servers infos : " << std::endl;
			for (size_t i = 0; i < registry.size(); i++)
			{
				std::cout << "servers (" << i + 1 << "): " << "host: " << registry[i]._host << "         " << " port: "<< registry[i]._port << "         " << "  fd : "<< registry[i]._listen_socket << std::endl;
			}
			webserver.run();
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << "EXCEPTION : "<< e.what() << std::endl;
	}
	return 0;
}
