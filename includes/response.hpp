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
	
	void get(const Request &request);

	void sendResponse(int clientSocket);
	void serveResponse(const Request &request);
	void serveFile(std::string url, std::map<int, std::string> &errorPages);
	void serveDirectory(std::string url, std::map<int, std::string> &errorPages, Location const &location);
	void serveDirectoryAutoIndex(std::string url, std::map<int, std::string> &errorPages);
	void redirect(std::string url);
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
	bool is_file(const char *path);
	bool is_directory(const char *path);

private:
	std::string _body;
	int _status;
	std::map<std::string, std::string> _headers;
};

std::ostream &operator<<(std::ostream &o, Response const &i);

#endif /* ******************************************************** RESPONSE_H */