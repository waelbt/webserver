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
#include <ctime>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>

#define MAX_BUFFER_SIZE 1024


class Request;

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
	bool getIsHeaderSent() const;
	bool getIsBodySent() const;
	bool getIsHeaderParsed() const;
	bool getIsRedirect() const;

	void setBody(std::string body);
	void setStatus(int status);
	void setHeader(std::string key, std::string value);
	void setIsHeaderSent(bool isHeaderSent);
	void setIsBodySent(bool isBodySent);
	void setIsFileOpned(bool isFileOpned);

	void post(const Request &request);
	void del(const Request &request);
	void get(const Request &request);

	//post methods
	void servePostFile(std::string url, std::map<int, std::string> &errorPages, Request const &request);
	void servePostDirectory(std::string url, std::map<int, std::string> &errorPages, Location const &location, Request const &request);

	void serveResponse(const Request &request);
	void serveFile(std::string url, std::map<int, std::string> &errorPages, Request const &request);
	void serveStaticFile(std::string url, std::map<int, std::string> &errorPages);
	void serveErrorPage(std::map<int, std::string> errorPages);
	void serveDirectory(std::string url, std::map<int, std::string> &errorPages, Location const &location, Request const &request);
	void serveDirectoryAutoIndex(std::string url, std::map<int, std::string> &errorPages);
	void redirect(std::string url);
	char **getENV(std::string url, const Request &request);
	void redirectLocation(const Request &request, std::string url);
	void addHTTPToEnvForCGI(std::map<std::string, std::string> &env, std::map<std::string, std::string> &headers);
	void executeCGI(std::string cgiPath, std::string binary, std::map<int, std::string> &errorPages, const Request &request);
	void parseResponseHeader(std::string responseHeader);
	void serveCGIFile(std::string cgiPath, std::map<int, std::string> &errorPages);
	int checkCGIStatus(std::map<int, std::string> &errorPages);
	void serveCGI(std::string url, const Request &request);
	std::string sendHeader();
	std::string toString() const;
	std::string intToString(int num) const;
	std::string getPathType(std::string url);
	std::string toUpperCase(std::string str);
	std::string getExtention(std::string url);
	std::string size_tToString(size_t size);
	std::string generateRandomFile(std::time_t result);
	bool is_file(const char *path);
	bool is_directory(const char *path);
	bool endWith(std::string const &value, std::string const &ending);
	bool isFileExists(const std::string &name);
	std::vector<std::string> split(const std::string &s, std::string delim);
	void initHTTPResponses();
	void reset();
	void delete_file(std::string path);
	void delete_directory(std::string path, const Request &request);
public :
	int _status;
	int _length;
	std::ifstream _file;
	bool _isCGIInProcess;
	bool _isCGIFinished;
	bool _isCGIParsed;
	bool _isFileOpned;
	bool _isHeaderSent;
	bool _isBodySent;
	bool _isHeaderParsed;
	bool _isRedirect;
	bool _isDeleted;
	pid_t _pid;
	std::string _body;
	std::string _generatedName;
	std::string _cgiOutput;
	std::map<std::string, std::string> _headers;
};

std::ostream &operator<<(std::ostream &o, Response const &i);

#endif /* ******************************************************** RESPONSE_H */