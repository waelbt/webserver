
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


#include "../includes/server.hpp"

s_cgi::s_cgi(std::string value)
{
    std::vector<std::string>  values(converter(value, token_to_string));
    std::vector<std::string>::iterator end = values.end();

    _exec = std::vector<std::string>(values.begin(), --end);
    _path = *end;
}

Route::Route() : _pattren("/"), _limit_except(), _cgi(), _upload()
{
}

void Route::InitPattern(std::string value)
{
    if (value != "")
        _pattren =  value; //maybe : i will check later if the path is valid or not
}

void Route::InitLimitExcept(std::string value)
{
    _limit_except = converter(value, token_to_string);
}

void Route::InitCgi(std::string value)
{
    _cgi.insert(_cgi.end(), s_cgi(value));
}

void Route::InitUpload(std::string value)
{
    if (value != "")
        _upload = value;
}

void Route::InitRedirect(std::string value)
{
    _redirect = converter(value, token_to_string);
}

Route::Route(const Server& Base, TokenVectsIter& begin, TokenVectsIter& end) : Server(Base), _pattren("/"), _limit_except(), _cgi(), _upload()
{
    initRoute MemberInit[10] = {&Route::InitPattern, &Route::InitLimitExcept, &Route::InitCgi, &Route::InitUpload, &Route::InitRoot, &Route::InitIndex, &Route::InitErrorPage, &Route::InitClienBodySize,&Route::InitAutoIndex, &Route::InitRedirect};
    static std::string keywords[11] = {"pattern", "limit_except", "cgi", "upload", "root", "index", "error_page", "client_max_body_size", "AutoIndex", "redirect", InvalidLocationKey};
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
        key = std::find(keywords, keywords + 10, directive[0].first);
        if (*key == InvalidLocationKey)
            throw CustomeExceptionMsg(directive[0].first + InvalidLocationKey);
        ((this->*MemberInit[key - keywords]))(directive[1].first);
    }
}

std::ostream& operator<<(std::ostream& o, Route obj)
{
    std::cout << "location  " << obj._pattren  << ": " << std::endl;
    std::cout << "  hosts: "<< obj._host << std::endl;
    std::cout << "  listen:";
    print_vec(obj._ports, "");
    std::cout << ";" << std::endl;
    std::cout << "  server name:";
    print_vec(obj._server_name, "");
    std::cout << "  root: " << obj._root << ";" << std::endl;
    std::cout << "  index: ";
    print_vec(obj._index, "");
    std::cout << ";" << std::endl;
    print_vec(obj._error_pages, "  error pages:");
    std::cout << "  client_max_body_size: " << obj._client_max_body_size << ";" << std::endl;
    std::cout << "  auto index: " << ((obj._AutoIndex) ? (std::cout << "true") :  (std::cout << "false")) << ";" << std::endl;
    std::cout << "  upload: " << obj._upload << ";" << std::endl;
    std::cout << "  limit_except: ";
    print_vec(obj._limit_except, "");
    std::cout << ";" << std::endl;
    print_vec(obj._cgi, "  cgi:");
    std::cout << std::endl;
    print_vec(obj._redirect, "  redirect:");
    return o;
}

Route::~Route()
{
}
