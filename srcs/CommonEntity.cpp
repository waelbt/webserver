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

#include "../includes/server.hpp"

CommonEntity::CommonEntity() : _root(), _index(), _error_pages(), _client_max_body_size(), _AutoIndex()
{
}

CommonEntity::CommonEntity(const CommonEntity& other)
{
    *this = other;
}

CommonEntity::CommonEntity(TokenVectsIter begin, TokenVectsIter end): _root(), _index(), _error_pages(), _client_max_body_size(), _AutoIndex()
{
    CommonEntityMethods MemberInit[5] = {&CommonEntity::InitRoot, &CommonEntity::InitIndex, &CommonEntity::InitErrorPage, &CommonEntity::InitClienBodySize, &CommonEntity::InitAutoIndex};
    static std::string keywords[6] = {"root", "index", "error_page", "client_body_size", "AutoIndex", InvalidSeverKey};
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
    return *this;
}

void CommonEntity::InitRoot(std::string value)
{
    _root = value;
}

void CommonEntity::InitIndex(std::string value)
{
    _index = converter(value, token_to_string);
}

void CommonEntity::InitErrorPage(std::string value)
{
    _error_pages.insert(_error_pages.end(), s_err_pages(value));
}

void CommonEntity::InitClienBodySize(std::string value)
{
    _client_max_body_size = to_integer(value);
}

void CommonEntity::InitAutoIndex(std::string value)
{
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

std::vector<s_err_pages>    CommonEntity::getErrorPages() const
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