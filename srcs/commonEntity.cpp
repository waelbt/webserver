/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommonEntity.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: waboutzo <waboutzo@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/23 19:29:06 by waboutzo          #+#    #+#             */
/*   Updated: 2023/05/27 19:05:49 by waboutzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserver.hpp"


CommonEntity::CommonEntity() : _root(), _index(), _error_pages(), _client_max_body_size(), _AutoIndex(false), _root_exits(false)
{
    default_error_pages(_error_pages);
}

CommonEntity::CommonEntity(const CommonEntity& other)
{
    *this = other;
}

CommonEntity::CommonEntity(TokenVectsIter begin, TokenVectsIter end): _root(), _index(), _error_pages(), _client_max_body_size(1073741824), _AutoIndex(false), _root_exits(false)
{
    CommonEntity::methods MemberInit[5] = {&CommonEntity::InitRoot, &CommonEntity::InitIndex, &CommonEntity::InitErrorPage, &CommonEntity::InitClienBodySize, &CommonEntity::InitAutoIndex};
    static std::string keywords[6] = {"root", "index", "error_page", "client_max_body_size", "AutoIndex", InvalidSeverKey};
    std::vector<TokenPair> directive;
    std::string *key;

    default_error_pages(_error_pages);
    while (++begin < end)
    {
        if (begin->second == END_BLOCK || begin->second == BLOCK)
            break;
        directive = SplitValues(begin->first, is_directive);
        if (directive.size() != 2)
            throw CustomeExceptionMsg(DirErr);
        std::for_each(directive.begin(), directive.end(), string_trim);
        key = std::find(keywords, keywords + 5, directive[0].first);
        if (*key != InvalidSeverKey)
            ((this->*MemberInit[key - keywords]))(directive[1].first);
    }
}

CommonEntity& CommonEntity::operator=(const CommonEntity& other)
{
    this->_root = other._root;
    this->_index = other._index;
    this->_error_pages = other._error_pages;
    this->_client_max_body_size = other._client_max_body_size;
    this->_AutoIndex = other._AutoIndex;
    this->_root_exits = other._root_exits;
    return *this;
}

void CommonEntity::InitRoot(std::string value)
{
    _root = value;
    _root_exits = true;
}

void CommonEntity::InitIndex(std::string value)
{
    _index = converter(value, TokenToString());
}

void CommonEntity::InitErrorPage(std::string value)
{
    std::vector<std::string>  values(converter(value, TokenToString()));

    if (value.size() < 2)
        throw CustomeExceptionMsg("you should at least set a stat code associated with a error page");
    std::string page = values[values.size() - 1];
    for (size_t index = 0; index < (values.size() - 1); index++)
        _error_pages[to_integer(values[index])] = page;
}

void CommonEntity::InitClienBodySize(std::string value)
{
    _client_max_body_size = to_integer(value);
}

void CommonEntity::InitAutoIndex(std::string value)
{
    if (value != "off" && value != "on")
        throw CustomeExceptionMsg("auto index should be `on` of `off`");
    (value == "on") ? _AutoIndex = true : _AutoIndex = false;
}

std::string CommonEntity::getRoot() const
{
    return this->_root;
}

std::vector<std::string>    CommonEntity::getIndex() const
{
    return this->_index;
}

std::map<int, std::string>    CommonEntity::getErrorPages() const
{
    return this->_error_pages;
}

size_t  CommonEntity::getClientMaxBodySize() const
{
    return this->_client_max_body_size;
}

bool    CommonEntity::getAutoIndex() const
{
    return this->_AutoIndex;
}