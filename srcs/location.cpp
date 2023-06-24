
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


#include "../includes/webserver.hpp"

Location::Location() : _pattren(), _limit_except(), _cgi(), _upload(), _pattren_exists(false)
{
}


Location::Location(const Location& other) : CommonEntity(other)
{
    this->_pattren = other._pattren;
    this->_limit_except = other._limit_except;
    this->_cgi = other._cgi;
    this->_upload = other._upload;
    this->_redirect = other._redirect;
    this->_pattren_exists = other._pattren_exists;
}


Location& Location::operator=(const Location& other)
{
    this->CommonEntity::operator=(other);
    this->_pattren = other._pattren;
    this->_limit_except = other._limit_except;
    this->_cgi = other._cgi;
    this->_upload = other._upload;
    this->_redirect = other._redirect;
    this->_pattren_exists = other._pattren_exists;
    return *this;
}

void Location::InitPattern(std::string value)
{
    _pattren =  value;
    _pattren_exists = true;
}

void Location::InitLimitExcept(std::string value)
{
    _limit_except = converter(value, TokenToString());
}

void Location::InitCgi(std::string value)
{
    std::vector<std::string>  values(converter(value, TokenToString()));
    if (values.size() != 2)
        throw CustomeExceptionMsg("invalid syntax for cgi (exec_file, path)");
    _cgi[values[0]] = values[1];
}

void Location::InitUpload(std::string value)
{
    if (value != "")
        _upload = value;
}

void Location::InitRedirect(std::string value)
{
    _redirect = value;
}

Location::Location(const CommonEntity& Base, TokenVectsIter& begin, TokenVectsIter& end) : CommonEntity(Base), _pattren("/"), _limit_except(), _cgi(), _upload(), _pattren_exists(false)
{
    Location::methods       init[10] = {&Location::InitPattern, &Location::InitLimitExcept, &Location::InitCgi, &Location::InitUpload, &Location::InitRoot, &Location::InitIndex, &Location::InitErrorPage, &Location::InitClienBodySize,&Location::InitAutoIndex, &Location::InitRedirect};
    static std::string      keywords[11] = {"pattern", "limit_except", "cgi", "upload", "root", "index", "error_page", "client_max_body_size", "AutoIndex", "redirect", InvalidLocationKey};
    std::vector<TokenPair>  directive;
    std::string             *key;

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
    }
    if (!_root_exits)
        throw CustomeExceptionMsg("you should explicitly define root directive -_-");
    if (!_pattren_exists)
        throw CustomeExceptionMsg("you should explicitly define pattern directive -_-");
}


std::string Location::getPattren() const
{
    return _pattren;
}

std::vector<std::string> Location::getLimit_except() const
{
    return _limit_except;
}

std::map<std::string, std::string> Location::getCgi() const
{
    return _cgi;
}

std::string Location::getUpload() const
{
    return _upload;
}

std::string Location::getRedirect() const
{
    return _redirect;
}

void print_error_page(const std::pair<int, std::string>& pair)
{
    std::cout << "  error_page: "<< pair.first << "  " << pair.second << ";"<< std::endl;
}

void print_cgi(const std::pair<std::string, std::string>& pair)
{
    std::cout << "  cgi: "<< pair.first << "  " << pair.second << ";"<< std::endl;
}

std::ostream& operator<<(std::ostream& o, Location obj)
{
    std::cout << "location  " << obj.getPattren()  << ": " << std::endl;
    std::cout << "  root: " << obj.getRoot() << ";" << std::endl;
    std::cout << "  index: ";
    print_vec(obj.getIndex(), "");
    std::cout << ";" << std::endl;
    std::map<int, std::string> error_pages = obj.getErrorPages();
    std::for_each(error_pages.begin(), error_pages.end(), &print_error_page);
    std::cout << "  client_max_body_size: " << obj.getClientMaxBodySize() << ";" << std::endl;
    std::cout << "  auto index: " << ((obj.getAutoIndex()) ? (std::cout << "true") :  (std::cout << "false")) << ";" << std::endl;
    std::cout << "  upload: " << obj.getUpload() << ";" << std::endl;
    std::cout << "  limit_except: ";
    print_vec(obj.getLimit_except() , "");
    std::cout << ";" << std::endl;
    std::map<std::string, std::string> cgi = obj.getCgi();
    std::for_each(cgi.begin(), cgi.end(), &print_cgi);
    std::cout << std::endl;
    std::cout << "  limit_except: " << obj.getRedirect() << std::endl;
    return o;
}

Location::~Location()
{
}
