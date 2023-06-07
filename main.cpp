/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: waboutzo <waboutzo@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 15:38:53 by waboutzo          #+#    #+#             */
/*   Updated: 2023/05/27 19:06:51 by waboutzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/server.hpp"


std::vector<Server> fill_servers(std::string content)
{
	TokenVects 									data(SplitValues(content));
	std::vector<Server> 						servers;
	std::pair<TokenVectsIter, TokenVectsIter>	it(std::make_pair(data.begin(),  data.end()));

	while (it.first < it.second)
	{
		if (it.first->second == BLOCK)
		{
			string_trim(*(it.first));
			if (it.first->first == "server")
				servers.insert(servers.end(), Server(Configuration(it.first, it.second)));
			else
				throw CustomeExceptionMsg(it.first->first + BlockErro);
		}
		else if ((it.first->second != END))
			throw CustomeExceptionMsg(it.first->first + BlockErro);
		it.first++;
	}
	if (servers.empty())
		throw CustomeExceptionMsg(EmptyFile);
	return servers;
}

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

int main(int ac, char **av)
{
	try
	{
		//map of servers each server had a key which is he's socket
		std::string content(((ac != 2) ? OpenPath() : OpenPath(av[1])));
		std::vector<Server> servers;

		// Server::clear_set();
		servers = fill_servers(content);
		servers[0].run();
	}
	catch(const std::exception& e)
	{
		std::cerr << "EXCEPTION : "<< e.what() << std::endl;
	}
	return 0;
}
