/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: waboutzo <waboutzo@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/23 19:29:06 by waboutzo          #+#    #+#             */
/*   Updated: 2023/05/27 19:05:49 by waboutzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Configuration.hpp"

ServerEntity::ServerEntity() : _host(), _ports(), _server_name()
{
}

ServerEntity::ServerEntity(TokenVectsIter& begin, TokenVectsIter& end)  : _host(), _ports(), _server_name()
{
    int             Count;
    CommonEntity    tmp(begin, end);

    initAttributes(begin, end);
    string_trim(*(begin));
    Count = 0;
    while(begin->first == "location")
    {
        _locations.insert(_locations.end(), Location(tmp, begin, end));
        begin++;
        string_trim(*begin);
        Count++;
    }
    if (!Count || begin->second != END_BLOCK)
		throw CustomeExceptionMsg((!Count) ? NOROUTE : ServerError);
}



void ServerEntity::initAttributes(TokenVectsIter& begin, TokenVectsIter& end)
{
    static std::string keywords[9] = {"host", "listen", "server_name", "root", "index", "error_page", "client_body_size", "AutoIndex", InvalidSeverKey};
    ServerEntity::methods MemberInit[3] = {&ServerEntity::InitHost, &ServerEntity::InitPort, &ServerEntity::InitServerName};
    std::vector<TokenPair> directive;
    std::string *key;

    while (++begin < end)
    {
        if (begin->second == END_BLOCK || begin->second == BLOCK)
            break;
        directive = SplitValues(begin->first, is_directive);
        if (directive.size() != 2)
            throw CustomeExceptionMsg(DirErr);
        std::for_each(directive.begin(), directive.end(), string_trim);
        key = std::find(keywords, keywords + 8, directive[0].first);
        if (*key == InvalidSeverKey)
            throw CustomeExceptionMsg(directive[0].first + InvalidSeverKey);
        if ((key - keywords) < 3)
            ((this->*MemberInit[key - keywords]))(directive[1].first);
    }
}


ServerEntity::ServerEntity(const ServerEntity& other)
{
    *this = other;
}

ServerEntity& ServerEntity::operator=(const ServerEntity& other)
{
    _host = other._host;
    _ports = other._ports;
    _server_name = other._server_name;
    _locations = other._locations;
    return *this;
}

void ServerEntity::InitHost(std::string value)
{
    _host = value;
}


void ServerEntity::InitPort(std::string value)
{
    std::vector<std::string>  values(converter(value, token_to_string));

    std::transform(values.begin(), values.end(), std::back_inserter(_ports), to_integer);
}

void ServerEntity::InitServerName(std::string value)
{
    _server_name = converter(value, token_to_string);
}

std::string ServerEntity::getHost() const
{
    return this->_host;
}

std::vector<int>    ServerEntity::getPorts() const
{
    return this->_ports;
}

std::vector<std::string>    ServerEntity::getServerNames() const
{
    return this->_server_name;
}


std::vector<Location>   ServerEntity::getLocations() const
{
    return _locations;
}

std::ostream& operator<<(std::ostream& o, ServerEntity obj)
{
    std::cout << "ServerEntity:" << std::endl;
    std::cout << " hosts: "<< obj.getHost() << std::endl;
    std::cout << " listen:";
    print_vec(obj.getPorts(), "");
    std::cout << ";" << std::endl;
    std::cout << " server name:";
    print_vec(obj.getServerNames(), "");
    std::cout << ";\n" << std::endl;
    print_vec(obj.getLocations(), "");
    return o;
}



ServerEntity::~ServerEntity()
{}