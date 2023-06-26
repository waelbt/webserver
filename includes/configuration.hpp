/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configuration.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: waboutzo <waboutzo@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/23 16:35:07 by waboutzo          #+#    #+#             */
/*   Updated: 2023/06/26 02:39:54 by waboutzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <exception>
#include <algorithm>
#include <fstream>
#include <stack>
#include <stdlib.h>
#include <map>
#include <sstream>
#include <fcntl.h>
#include <queue>
#include <string>
#include <exception>
#include <vector>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

#define NOROUTE				"define a server without location is forbidden"
#define ServerError			": can't be in the server scope"
#define BlockErro			": can't be in the main scope"
#define DefaultPath			"./conf/sample.conf"
#define EmptyFile			"empty config file"
#define FileFailed			"failed to open the file : "
#define SyntaxError			"Syntax error, please insert a valid config file format"
#define CreationFailed 		"can't create a server object without having a block token -_-"
#define InvalidSeverKey		" inappropriate for the server context"
#define InvalidLocationKey 	" inappropriate for the location context"
#define InvalidPort			"invalid ports number"
#define MaxBodySize			"you reach the max body size the can be send by client"
#define DirErr				"invalid directive syntax"
#define MISSINGPORTHOST		"you should explicitly set the port and the host directives"
#define EmptyDirective 		"empty directive, set a valid value pls"
#define EmptyLocation 		"empty lcoation context !!!!!"

//EntityType
#define KEY ':'
#define VAL ';'
#define END '\0'
#define BLOCK '{'
#define END_BLOCK '}'

#define BACKLOG 32767
#define MAX_REQUEST_SIZE 65536

typedef char 								EntityType;
typedef std::string::iterator 				StrIter;
typedef	std::pair<std::string,EntityType>		TokenPair;
typedef std::pair<std::string,std::string>	StringPair;
typedef	std::vector<StringPair>				ConfEntity;
typedef	std::vector<StringPair>::iterator	ConfEntityIer;
typedef std::vector<TokenPair>				TokenVects;
typedef TokenVects::iterator	             TokenVectsIter;



struct TokenToString {
	std::string  operator()(TokenPair& pair);
};

template <typename T>
std::string to_string(T num)
{
	std::stringstream ss;

	ss << num;
	return ss.str();
}

bool							is_space(const char& c);
bool							is_symbol(const char& c);
bool							is_semicolon(const char& c);
void							string_trim(TokenPair& pair);
void							is_integer(std::string num);
bool							is_directive(const char& c);
long long						to_integer(const std::string& string);
TokenPair 						selectToken(StrIter& begin, const StrIter& end, int& level, bool (*func)(const char&));
std::string 					token_to_string(TokenPair& pair);
std::vector<TokenPair> 			SplitValues(std::string value, bool (*func)(const char&) = is_symbol);
void 							default_error_pages(std::map<int, std::string>& error_pages);
template <typename T>
void print_vec( const std::vector<T>& vec, std::string name)
{
    typedef typename std::vector<T>::const_iterator const_iterator;

    for (const_iterator it = vec.begin(); it != vec.end(); it++)
    {
        std::cout << name;
        std::cout << " " << *it;
    }
}


template <typename func>
std::vector<std::string> converter(const std::string& content, func function)
{
    std::vector<std::string> values;
    std::vector<TokenPair> tokens;

    tokens = SplitValues(content, is_semicolon);
	for (std::vector<TokenPair>::iterator it = tokens.begin(); it != tokens.end(); it++)
	{
		values.insert(values.end(), function(*it));
	}
    return values;
}

class CustomeExceptionMsg : public std::exception {
	protected:
    	std::string _message;
	public:
        CustomeExceptionMsg();
    	CustomeExceptionMsg(const std::string& message);
		const char* what() const throw();
		virtual ~CustomeExceptionMsg() throw();
};

class CommonEntity
{
	public:
		typedef void (CommonEntity::*methods)(std::string);
	protected:
		std::string											_root;
		std::vector<std::string> 							_index;
		std::map<int, std::string> 	 						_error_pages;
		size_t 												_client_max_body_size;
		bool 												_AutoIndex;
		bool												_root_exits;
	public:
		CommonEntity();
		CommonEntity(TokenVectsIter begin, TokenVectsIter end);
		CommonEntity(const CommonEntity& other);
		CommonEntity& operator=(const CommonEntity& other);
		void 						InitRoot(std::string value);
		void 						InitIndex(std::string value);
		void 						InitErrorPage(std::string value);
		void 						InitClienBodySize(std::string value);
		void 						InitAutoIndex(std::string value);
		std::string					getRoot() const;
		std::vector<std::string> 	getIndex() const;
		std::map<int, std::string>	getErrorPages() const;
		size_t 						getClientMaxBodySize() const;
		bool 						getAutoIndex() const;
};

class Location : public CommonEntity
{
	public:
		typedef void (Location::*methods)(std::string);
	private:
		std::string _pattren;
		std::vector<std::string> _limit_except;
		std::map<std::string, std::string> _cgi;
		std::string _upload;
		std::string _redirect;
		bool		_pattren_exists;
	public:
		Location();
		Location(const CommonEntity& base, TokenVectsIter& begin, TokenVectsIter& end);
		Location(const Location& other);
		Location& 					operator=(const Location& other);
		void 						InitPattern(std::string value);
		void 						InitLimitExcept(std::string value);
		void 						InitCgi(std::string value);
		void 						InitUpload(std::string value);
		void 						InitRedirect(std::string value);
		std::string 				getPattren() const;
		std::vector<std::string> 	getLimit_except() const;
		std::map<std::string, std::string> 			getCgi() const;
		std::string 				getUpload() const;
		std::string 				getRedirect() const;
		~Location();
		friend std::ostream& operator<<(std::ostream& o, Location obj);
};

class Configuration
{
	public:
		typedef void (Configuration::*methods)(std::string);
	private:
		std::string 										_host;
		std::string 										_port;
		bool 												_host_exists;
		bool 												_port_exists;
		// static std::map<std::string, std::string>			_host_port_map;
		std::string											_server_name;
		std::vector<Location>								_locations;
	public:
		Configuration();
		// Configuration operator()(std::string host, std::string  port);
		Configuration(TokenVectsIter& begin, TokenVectsIter& end);
		void initAttributes(TokenVectsIter& begin, TokenVectsIter& end);
        Configuration(const Configuration& other);
		Configuration& operator=(const Configuration& other);
		void InitHost(std::string value);
		void InitPort(std::string value);
		void InitServerName(std::string value);
		// static	void check_dup(std::string host, std::string port);
		std::string 				getHost() const;
		std::string					getPort() const;
		std::string	getServerNames() const;
		std::vector<Location>		getLocations() const;
		friend std::ostream& operator<<(std::ostream& o, Configuration obj);
		~Configuration();
};



typedef std::vector<Configuration> ConfVec;