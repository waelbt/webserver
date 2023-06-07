/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: waboutzo <waboutzo@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/23 16:35:07 by waboutzo          #+#    #+#             */
/*   Updated: 2023/05/27 17:07:05 by waboutzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>



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

#define MAX_PENDING_CNX 10
#define MAX_REQUEST_SIZE 2047

typedef char 								EntityType;
typedef std::string::iterator 				StrIter;
typedef	std::pair<std::string,EntityType>		TokenPair;
typedef std::pair<std::string,std::string>	StringPair;
typedef	std::vector<StringPair>				ConfEntity;
typedef	std::vector<StringPair>::iterator	ConfEntityIer;
typedef std::vector<TokenPair>				TokenVects;
typedef TokenVects::iterator	TokenVectsIter;



typedef int	SOCKET;
typedef struct addrinfo s_addrinfo;
typedef struct sockaddr_storage s_sockaddr_storage;

struct PortValidator {
    void operator()(int port);
};


struct TokenToString {
	std::string  operator()(TokenPair& pair);
};

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

SOCKET create_socket(const std::string& host, const std::string& port);

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

struct s_cgi
{
	std::vector<std::string> _exec;
	std::string _path;

	s_cgi(std::string value);
	friend std::ostream& operator<<(std::ostream& o, s_cgi obj);
};

struct s_err_pages
{
	std::vector<int> _status;
	std::string _page;

	s_err_pages(std::string value);
	friend std::ostream& operator<<(std::ostream& o, s_err_pages obj);
};

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
		std::vector<s_err_pages> 	 						_error_pages;
		size_t 												_client_max_body_size;
		bool 												_AutoIndex;
	public:
		CommonEntity();
		CommonEntity(TokenVectsIter begin, TokenVectsIter end);
		CommonEntity(const CommonEntity& other);
		CommonEntity& operator=(const CommonEntity& other);
		void InitRoot(std::string value);
		void InitIndex(std::string value);
		void InitErrorPage(std::string value);
		void InitClienBodySize(std::string value);
		void InitAutoIndex(std::string value);
		std::string					getRoot() const;
		std::vector<std::string> 	getIndex() const;
		std::vector<s_err_pages> 	getErrorPages() const;
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
		std::vector<s_cgi> _cgi;
		std::string _upload;
		std::vector<std::string> _redirect;
	public:
		Location();
		Location(const CommonEntity& base, TokenVectsIter& begin, TokenVectsIter& end);
		void InitPattern(std::string value);
		void InitLimitExcept(std::string value);
		void InitCgi(std::string value);
		void InitUpload(std::string value);
		void InitRedirect(std::string value);
		std::string getPattren() const;
		std::vector<std::string> getLimit_except() const;
		std::vector<s_cgi> getCgi() const;
		std::string getUpload() const;
		std::vector<std::string> getRedirect() const;
		friend std::ostream& operator<<(std::ostream& o, Location obj);
		~Location();
};

class Configuration
{
	public:
		typedef void (Configuration::*methods)(std::string);
	private:
		std::string 										_host;
		std::string 										_port;
		std::vector<std::string>							_server_name;
		std::vector<Location>								_locations;
	public:
		Configuration();
		Configuration(TokenVectsIter& begin, TokenVectsIter& end);
		void initAttributes(TokenVectsIter& begin, TokenVectsIter& end);
        Configuration(const Configuration& other);
		Configuration& operator=(const Configuration& other);
		void InitHost(std::string value);
		void InitPort(std::string value);
		void InitServerName(std::string value);
		std::string 				getHost() const;
		std::string					getPort() const;
		std::vector<std::string>	getServerNames() const;
		std::vector<Location>		getLocations() const;
		friend std::ostream& operator<<(std::ostream& o, Configuration obj);
		~Configuration();
};

// class Client {
// 	private:
// 		SOCKET _server_socket;
// 		socklen_t _address_length;
// 		s_sockaddr_storage _address;
// 		SOCKET _socket;
// 		char _request[MAX_REQUEST_SIZE + 1];
// 		int _received;
// 	public:
// 		Client(SOCKET server_socket);
// };

class Server
{
    private:
		static size_t	_counter;
		size_t	_id;
        Configuration _conf;
		SOCKET _listen_sockets;
    public:
		Server(const Configuration& conf);
		// void setup();
		void run();
        void showConfig() const;
};
