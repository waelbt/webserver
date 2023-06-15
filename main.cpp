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
	try
	{
		std::string content(((ac != 2) ? OpenPath() : OpenPath(av[1])));

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
