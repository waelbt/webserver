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

Server::Server() : _host(), _ports(), _server_name(), _root(), _index(), _error_pages(), _client_max_body_size(), _AutoIndex()
{
}

Server::Server(TokenVectsIter& begin, TokenVectsIter& end)  : _host(), _ports(), _server_name(), _root(), _index(), _error_pages(), _client_max_body_size(), _AutoIndex()
{
    initserver MemberInit[8] = {&Server::InitHost, &Server::InitPort, &Server::InitServerName, &Server::InitRoot, &Server::InitIndex, &Server::InitErrorPage, &Server::InitClienBodySize, &Server::InitAutoIndex};
    static std::string keywords[9] = {"host", "listen", "server_name", "root", "index", "error_page", "client_body_size", "AutoIndex", InvalidSeverKey};
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
        ((this->*MemberInit[key - keywords]))(directive[1].first);
    }
}


Server::Server(const Server& other)
{
    *this = other;
}

Server& Server::operator=(const Server& other)
{
    _host = other._host;
    _ports = other._ports;
    _server_name = other._server_name;
    _root = other._root;
    _index = other._index;
    _error_pages = other._error_pages;
    _client_max_body_size = other._client_max_body_size;
    _AutoIndex = other._AutoIndex;
    return *this;
}

void Server::InitHost(std::string value)
{
    _host = value;
}


void Server::InitPort(std::string value)
{
    std::vector<std::string>  values(converter(value, token_to_string));

    std::transform(values.begin(), values.end(), std::back_inserter(_ports), to_integer);
}

void Server::InitServerName(std::string value)
{
    _server_name = converter(value, token_to_string);
}

void Server::InitRoot(std::string value)
{
    _root = value;
}


void Server::InitIndex(std::string value)
{
    _index = converter(value, token_to_string);
}

void Server::InitErrorPage(std::string value)
{
    _error_pages.insert(_error_pages.end(), s_err_pages(value));
}

void Server::InitClienBodySize(std::string value)
{
    _client_max_body_size = to_integer(value);
}

void Server::InitAutoIndex(std::string value)
{
    (value == "on") ? _AutoIndex = true : _AutoIndex = false;
}

std::string Server::getHost() const
{
    return this->_host;
}

std::vector<int>    Server::getPorts() const
{
    return this->_ports;
}

std::vector<std::string>    Server::getServerNames() const
{
    return this->_server_name;
}

std::string Server::getRoot() const
{
    return this->_root;
}

std::vector<std::string>    Server::getIndex() const
{
    return this->_index;
}

std::vector<s_err_pages>    Server::getErrorPages() const
{
    return this->_error_pages;
}

size_t  Server::getClientMaxBodySize() const
{
    return this->_client_max_body_size;
}

bool    Server::getAutoIndex() const
{
    return this->_AutoIndex;
}

s_err_pages::s_err_pages(std::string value)
{
    std::vector<std::string>  values(converter(value, token_to_string));
    std::vector<std::string>::iterator end = values.end();

    std::transform(values.begin(), --end, std::back_inserter(this->_status), to_integer);
    std::for_each(_status.begin(), _status.end(), PortValidator());
    _page = *end;
}

Server::~Server()
{}