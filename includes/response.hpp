#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sstream>
#include <fstream>
#include <map>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include "request.hpp"

#define MAX_BUFFER_SIZE 1024

class Response
{

public:
	Response();
	Response(Response const &src);
	~Response();

	Response &operator=(Response const &rhs);

	std::string getBody() const;
	int getStatus() const;
	std::map<std::string, std::string> getHeaders() const;

	void setBody(std::string body);
	void setStatus(int status);
	void setHeader(std::string key, std::string value);

	void post(const Request &request);
	void del(const Request &request);
	void get(const Request &request);

	void serveResponse(const Request &request);
	void serveFile(std::string url, std::map<int, std::string> &errorPages, Request const &request);
	void serveStaticFile(std::string url, std::map<int, std::string> &errorPages);
	void serveErrorPage(std::map<int, std::string> &errorPages);
	void serveDirectory(std::string url, std::map<int, std::string> &errorPages, Location const &location);
	void serveDirectoryAutoIndex(std::string url, std::map<int, std::string> &errorPages);
	void redirect(std::string url);
	char **getENV(const Request &request);
	void addHTTPToEnvForCGI(std::map<std::string, std::string> &env, std::map<std::string, std::string> &headers);
	void executeCGI(std::string cgiPath, std::string binary, char **envp, std::map<int, std::string> &errorPages);
	void parseResponseHeader(std::string responseHeader);
	void serveCGI(std::string url, const Request &request);
	// void error(int clientSocket);
	// void serveDirectory(std::string url, int clientSocket);
	// void serveCgi(const std::string &url, int clientSocket);
	// void serveGet(std::string url, int clientSocket);
	// void servePost(std::string url, std::map<std::string, std::string> data, int clientSocket);
	// void serveDelete(std::string url, int clientSocket);
	// void clear();
	// std::string chunkedEncoding(std::string body);
	std::string toString() const;
	std::string intToString(int num) const;
	std::string getPathType(std::string url);
	std::string toUpperCase(std::string str);
	std::string getExtention(std::string url);
	bool is_file(const char *path);
	bool is_directory(const char *path);
	bool endWith(std::string const &value, std::string const &ending);
	bool isFileExists(const std::string &name);
	std::vector<std::string> split(const std::string &s, std::string delim);

private : 
	std::string _body;
	int _status;
	std::map<std::string, std::string> _headers;
};

std::ostream &operator<<(std::ostream &o, Response const &i);

#endif /* ******************************************************** RESPONSE_H */