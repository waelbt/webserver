/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: waboutzo <waboutzo@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 15:38:53 by waboutzo          #+#    #+#             */
/*   Updated: 2023/05/27 16:58:49 by waboutzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Configuration.hpp"

Configuration::Configuration(std::string content)
{
	int											location;
	Server              						*server;
	TokenVects 									data;
	std::pair<TokenVectsIter, TokenVectsIter>	it;

	server = NULL;
	data = SplitValues(content);
	it = std::make_pair(data.begin(),  data.end());
	while (it.first < it.second)
	{
		if (it.first->second == BLOCK)
		{
			string_trim(*(it.first));
			if (it.first->first == "server")
			{
				delete server;
				server = new Server(it.first, it.second);
				string_trim(*(it.first));
				location = 0;
				while(it.first->first == "location")
				{
					_routes.insert(_routes.end(), Route(server, it.first, it.second));
					it.first++;
					string_trim(*(it.first));
					location++;
				}
				if (!location || it.first->second != END_BLOCK){
					delete server; throw CustomeExceptionMsg((!location) ? NOROUTE : ServerError);}
			}
			else  {
				delete server; throw CustomeExceptionMsg(it.first->first + BlockErro);}
		}
		else if ((it.first->second != END))  {
			delete server; throw CustomeExceptionMsg(it.first->first + BlockErro);}
		it.first++;
	}
	delete server;
	if (_routes.empty())
		throw CustomeExceptionMsg(EmptyFile);
}

std::vector<Route> Configuration::getRoutes() const
{
	return _routes;
}


void Configuration::showdata() const
{
	for (std::vector<Route>::const_iterator it = _routes.begin(); it != _routes.end(); it++)
		std::cout << *it << std::endl;
}