
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


Route::Route() : _pattren(), _limit_except(), _cgi(), _upload()
{
}

void Route::InitPattern(std::string value)
{
   _pattren = value; //maybe : i will check later if the path is valid or not
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
    _upload = value;
}

Route::Route(Server *Base, TokenVectsIter& begin, TokenVectsIter& end) : Server(*Base), _pattren(), _limit_except(), _cgi(), _upload()
{
    initRoute MemberInit[9] = {&Route::InitPattern, &Route::InitLimitExcept, &Route::InitCgi, &Route::InitUpload, &Route::InitRoot, &Route::InitIndex, &Route::InitErrorPage, &Route::InitClienBodySize,&Route::InitAutoIndex};
    static std::string keywords[10] = {"pattern", "limit_except", "cgi", "upload", "root", "index", "error_page", "client_body_size", "AutoIndex", InvalidLocationKey};
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
        key = std::find(keywords, keywords + 9, directive[0].first);
        if (*key == InvalidLocationKey)
            throw CustomeExceptionMsg(directive[0].first + InvalidLocationKey);
        ((this->*MemberInit[key - keywords]))(directive[1].first);
    }
}


std::ostream& operator<<(std::ostream& o, Route obj)
{
    std::cout << "location        " << obj._pattren  << ": " << std::endl;
    std::cout << "  hosts: "<< obj._host << std::endl;
    std::cout << "  listen:";
    for (std::vector<size_t>::iterator it = obj._ports.begin(); it != obj._ports.end(); it++)
        std::cout << *it << "    ";
    std::cout << std::endl;
    std::cout << "  server name:";
    std::cout << obj._server_name << std::endl;
    std::cout << "  root:";
    std::cout << obj._root << std::endl;
     std::cout << "  index:";
    for (std::vector<std::string>::iterator it = obj._index.begin(); it != obj._index.end(); it++)
        std::cout << *it << "    ";
    std::cout << std::endl;
     std::cout << "  error pages:";
    for (std::vector<s_err_pages>::iterator it = obj._error_pages.begin(); it != obj._error_pages.end(); it++)
        std::cout << *it << "    ";
    std::cout << "  client_max_body_size:";
    std::cout << obj._client_max_body_size << std::endl;
    std::cout << "  auto index:";
    (obj._AutoIndex) ? (std::cout << "true") :  (std::cout << "false");
    std::cout << "  upload:" << obj._upload << std::endl;
    std::cout << std::endl;
    return o;
}

Route::~Route()
{
}
