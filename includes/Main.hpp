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
#include <unistd.h>
#include <string>
#include <exception>
#include <vector>

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

//EntityType
#define KEY ':'
#define VAL ';'
#define END '\0'
#define BLOCK '{'
#define END_BLOCK '}'

typedef char 								EntityType;
typedef std::string::iterator 				StrIter;
typedef	std::pair<std::string,EntityType>		TokenPair;
typedef std::pair<std::string,std::string>	StringPair;
typedef	std::vector<StringPair>				ConfEntity;
typedef	std::vector<StringPair>::iterator	ConfEntityIer;
typedef std::vector<TokenPair>				TokenVects;
typedef TokenVects::iterator	TokenVectsIter;



class CustomeExceptionMsg : public std::exception {
	protected:
    	std::string _message;
	public:
        CustomeExceptionMsg();
    	CustomeExceptionMsg(const std::string& message);
		const char* what() const throw();
		virtual ~CustomeExceptionMsg() throw();
};

bool							is_space(const char& c);
bool							is_symbol(const char& c);
bool							is_semicolon(const char& c);
void							string_trim(TokenPair& pair);
void							string_trim(TokenPair& pair);
bool							is_directive(const char& c);
long long						to_integer(const std::string& string);
TokenPair 						selectToken(StrIter& begin, const StrIter& end, int& level, bool (*func)(const char&));
std::string 					token_to_string(TokenPair& pair);
std::vector<TokenPair> 			SplitValues(std::string value, bool (*func)(const char&) = is_symbol);

template <typename T>
void print_vec( const std::vector<T>& vec, std::string name)
{
    typedef typename std::vector<T>::const_iterator const_iterator;

    for (const_iterator it = vec.begin(); it != vec.end(); it++)
    {
        std::cout << name;
        std::cout << " "<< *it;
    }    
}
struct s_cgi
{
	std::vector<std::string> _exec;
	std::string _path;

	s_cgi(std::string value);
};

struct s_err_pages
{
	std::vector<int> _status;
	std::string _page;

	s_err_pages(std::string value);
};


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

std::ostream& operator<<(std::ostream& o, s_err_pages obj);
std::ostream& operator<<(std::ostream& o, s_cgi obj);