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

#include "../../includes/Configuration.hpp"

ServerData::ServerData() : _host(), _ports(), _server_name()
{
}

ServerData::ServerData(TokenVectsIter& begin, TokenVectsIter& end)  : _host(), _ports(), _server_name()
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

void ServerData::initAttributes(TokenVectsIter& begin, TokenVectsIter& end)
{
    static std::string keywords[9] = {"host", "listen", "server_name", "root", "index", "error_page", "client_body_size", "AutoIndex", InvalidSeverKey};
    ServerData::methods MemberInit[3] = {&ServerData::InitHost, &ServerData::InitPort, &ServerData::InitServerName};
    std::vector<TokenPair> directive;
    std::string *key;
    size_t counter;

    counter = 0;
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
        {
            if (key - keywords < 2)
                counter++;
            ((this->*MemberInit[key - keywords]))(directive[1].first);
        }
    }
    if (counter != 2)
        throw CustomeExceptionMsg(MISSINGPORTHOST);
}


ServerData::ServerData(const ServerData& other)
{
    *this = other;
}

ServerData& ServerData::operator=(const ServerData& other)
{
    _host = other._host;
    _ports = other._ports;
    _server_name = other._server_name;
    _locations = other._locations;
    return *this;
}

void ServerData::InitHost(std::string value)
{
    _host = value;
}

void ServerData::InitPort(std::string value)
{
    std::vector<std::string>  _ports(converter(value, TokenToString()));

    if (_ports.empty())
        throw CustomeExceptionMsg(EmptyDirective);
    std::for_each(_ports.begin(), _ports.end(), is_integer);
    std::sort(_ports.begin(), _ports.end());
}

void ServerData::InitServerName(std::string value)
{
    _server_name = converter(value, TokenToString());
}

std::string ServerData::getHost() const
{
    return this->_host;
}

std::vector<std::string>    ServerData::getPorts() const
{
    return this->_ports;
}

std::vector<std::string>    ServerData::getServerNames() const
{
    return this->_server_name;
}


std::vector<Location>   ServerData::getLocations() const
{
    return _locations;
}

std::ostream& operator<<(std::ostream& o, ServerData obj)
{
    std::cout << "ServerData:" << std::endl;
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

ServerData::~ServerData()
{}