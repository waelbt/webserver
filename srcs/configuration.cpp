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

#include "../includes/server.hpp"

std::map<std::string, std::string> Configuration::_host_port_map;

Configuration::Configuration() : _host(), _port(), _server_name(), _host_exists(0), _port_exists(0), _locations()
{
}

Configuration::Configuration(TokenVectsIter& begin, TokenVectsIter& end)  : _host(), _port(), _server_name(), _host_exists(0), _port_exists(0), _locations()
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

void Configuration::initAttributes(TokenVectsIter& begin, TokenVectsIter& end)
{
    static std::string keywords[9] = {"host", "listen", "server_name", "root", "index", "error_page", "client_max_body_size", "AutoIndex", InvalidSeverKey};
    Configuration::methods MemberInit[3] = {&Configuration::InitHost, &Configuration::InitPort, &Configuration::InitServerName};
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
    if (!_host_exists || !_port_exists)
        throw CustomeExceptionMsg(MISSINGPORTHOST);
    // check_dup(_host, _port);
}

// void Configuration::check_dup(std::string host, std::string port)
// {
//     std::map<std::string, std::string>::iterator it = _host_port_map.find(host);
//     if (it != _host_port_map.end() && it->second == port)
//         throw CustomeExceptionMsg("you're not allowed to reuse the same port:host -__-");
//     _host_port_map[host] = port;
// }

Configuration::Configuration(const Configuration& other)
{
    *this = other;
}

Configuration& Configuration::operator=(const Configuration& other)
{
    _host = other._host;
    _port = other._port;
    _server_name = other._server_name;
    _port_exists = other._port_exists;
    _host_exists = other._host_exists;
    _locations = other._locations;
    return *this;
}

void Configuration::InitHost(std::string value)
{
    _host = value;
    _host_exists = true;
}

void Configuration::InitPort(std::string value)
{
    _port = value;
    long long check = to_integer(_port);
     if (check < 0 || check > 65535)
        throw CustomeExceptionMsg(InvalidPort);
    _port_exists = true;
}

void Configuration::InitServerName(std::string value)
{
    _server_name = converter(value, TokenToString());
}

std::string Configuration::getHost() const
{
    return this->_host;
}

std::string    Configuration::getPort() const
{
    return this->_port;
}

std::vector<std::string>    Configuration::getServerNames() const
{
    return this->_server_name;
}


std::vector<Location>   Configuration::getLocations() const
{
    return _locations;
}

std::ostream& operator<<(std::ostream& o, Configuration obj)
{
    std::cout << "Server:" << std::endl;
    std::cout << " hosts: "<< obj.getHost() << ";" << std::endl;
    std::cout << " listen:" << obj.getPort() << ";" << std::endl;
    std::cout << " server name:";
    print_vec(obj.getServerNames(), "");
    std::cout << ";\n" << std::endl;
    print_vec(obj.getLocations(), "");
    return o;
}

Configuration::~Configuration()
{}