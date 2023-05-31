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
	TokenVects 									data;
	std::pair<TokenVectsIter, TokenVectsIter>	it;

	data = SplitValues(content);
	it = std::make_pair(data.begin(),  data.end());
	while (it.first < it.second)
	{
		if (it.first->second == BLOCK)
		{
			string_trim(*(it.first));
			if (it.first->first == "server")
			{
				Server  server(it.first, it.second);
				string_trim(*(it.first));
				location = 0;
				while(it.first->first == "location")
				{
					_routes.insert(_routes.end(), Route(server, it.first, it.second));
					it.first++;
					string_trim(*(it.first));
					location++;
				}
				if (!location || it.first->second != END_BLOCK)
					throw CustomeExceptionMsg((!location) ? NOROUTE : ServerError);
			}
			else
				throw CustomeExceptionMsg(it.first->first + BlockErro);
		}
		else if ((it.first->second != END))
			throw CustomeExceptionMsg(it.first->first + BlockErro);
		it.first++;
	}
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