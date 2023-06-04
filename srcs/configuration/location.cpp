
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: waboutzo <waboutzo@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/23 19:29:06 by waboutzo          #+#    #+#             */
/*   Updated: 2023/05/23 22:44:41 by waboutzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../includes/Configuration.hpp"

s_cgi::s_cgi(std::string value)
{
    std::vector<std::string>  values(converter(value, TokenToString()));
    std::vector<std::string>::iterator end = values.end();

    _exec = std::vector<std::string>(values.begin(), --end);
    _path = *end;
}

Location::Location() : _pattren("/"), _limit_except(), _cgi(), _upload()
{
}

void Location::InitPattern(std::string value)
{
    if (value != "")
        _pattren =  value; //maybe : i will check later if the path is valid or not
}

void Location::InitLimitExcept(std::string value)
{
    _limit_except = converter(value, TokenToString());
}

void Location::InitCgi(std::string value)
{
    _cgi.insert(_cgi.end(), s_cgi(value));
}

void Location::InitUpload(std::string value)
{
    if (value != "")
        _upload = value;
}

void Location::InitRedirect(std::string value)
{
    _redirect = converter(value, TokenToString());
}

Location::Location(const CommonEntity& Base, TokenVectsIter& begin, TokenVectsIter& end) : CommonEntity(Base), _pattren("/"), _limit_except(), _cgi(), _upload()
{
    Location::methods init[10] = {&Location::InitPattern, &Location::InitLimitExcept, &Location::InitCgi, &Location::InitUpload, &Location::InitRoot, &Location::InitIndex, &Location::InitErrorPage, &Location::InitClienBodySize,&Location::InitAutoIndex, &Location::InitRedirect};
    static std::string keywords[11] = {"pattern", "limit_except", "cgi", "upload", "root", "index", "error_page", "client_max_body_size", "AutoIndex", "redirect", InvalidLocationKey};
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
        key = std::find(keywords, keywords + 10, directive[0].first);
        if (*key == InvalidLocationKey)
            throw CustomeExceptionMsg(directive[0].first + InvalidLocationKey);
        ((this->*init[key - keywords]))(directive[1].first);
        counter++;
    }
    if (!counter)
        throw CustomeExceptionMsg(EmptyLocation);
}


std::string Location::getPattren() const
{
    return _pattren;
}

std::vector<std::string> Location::getLimit_except() const
{
    return _limit_except;
}

std::vector<s_cgi> Location::getCgi() const
{
    return _cgi;
}

std::string Location::getUpload() const
{
    return _upload;
}

std::vector<std::string> Location::getRedirect() const
{
    return _redirect;
}

std::ostream& operator<<(std::ostream& o, Location obj)
{
    std::cout << "location  " << obj.getPattren()  << ": " << std::endl;
    std::cout << "  root: " << obj.getRoot() << ";" << std::endl;
    std::cout << "  index: ";
    print_vec(obj.getIndex(), "");
    std::cout << ";" << std::endl;
    print_vec(obj.getErrorPages(), "  error pages:");
    std::cout << "  client_max_body_size: " << obj.getClientMaxBodySize() << ";" << std::endl;
    std::cout << "  auto index: " << ((obj.getAutoIndex()) ? (std::cout << "true") :  (std::cout << "false")) << ";" << std::endl;
    std::cout << "  upload: " << obj.getUpload() << ";" << std::endl;
    std::cout << "  limit_except: ";
    print_vec(obj.getLimit_except() , "");
    std::cout << ";" << std::endl;
    print_vec(obj.getCgi(), "  cgi:");
    std::cout << std::endl;
    print_vec(obj.getRedirect(), "  redirect:");
    return o;
}

Location::~Location()
{
}
