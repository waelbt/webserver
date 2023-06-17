#include "../includes/response.hpp"

std::map<int, std::string> _httpResponses;

Response::Response(): _status(200), _isCGIInProcess(0), _isCGIFinished(0), _body("")
{
	_httpResponses[200] = "OK";
	_httpResponses[201] = "Created";
	_httpResponses[202] = "Accepted";
	_httpResponses[204] = "No Content";
	_httpResponses[301] = "Moved Permanently";
	_httpResponses[302] = "Found";
	_httpResponses[304] = "Not Modified";
	_httpResponses[400] = "Bad Request";
	_httpResponses[401] = "Unauthorized";
	_httpResponses[403] = "Forbidden";
	_httpResponses[404] = "Not Found";
	_httpResponses[405] = "Method Not Allowed";
	_httpResponses[413] = "Payload Too Large";
	_httpResponses[500] = "Internal Server Error";
	_httpResponses[501] = "Not Implemented";
	_httpResponses[505] = "HTTP Version Not Supported";
}

Response::Response(Response const &src) { *this = src; }

Response::~Response() {}

Response &Response::operator=(Response const &rhs)
{
	if (this != &rhs)
	{
		this->_body = rhs._body;
	}
	return *this;
}

std::string Response::intToString(int num) const
{
	std::ostringstream oss;
	oss << num;
	return oss.str();
}

std::string Response::getBody() const { return this->_body; }

void Response::setBody(std::string body) { this->_body = body; }

int Response::getStatus() const { return this->_status; }

void Response::setStatus(int status) { this->_status = status; }

std::map<std::string, std::string> Response::getHeaders() const { return this->_headers; }

void Response::setHeader(std::string key, std::string value) { this->_headers[key] = value; }

std::string Response::toString() const
{
	std::stringstream ss;
	ss << "HTTP/1.1 " << this->intToString(this->_status) << " " << _httpResponses[this->_status] << "\r\n";
	for (std::map<std::string, std::string>::const_iterator it = this->_headers.begin(); it != this->_headers.end(); ++it)
	{
		ss << it->first << ": " << it->second << "\r\n";
	}
	ss << "\r\n";
	ss << this->_body;
	return ss.str();
}

void Response::serveResponse(const Request &request)
{
	std::string method = request.getRequest().find("Method")->second;
	std::cout << method << " the path is " << request.getPath() <<std::endl;
	if (method == "GET")
		this->get(request);
	else if (method == "POST")
		this->post(request);
	else if (method == "DELETE")
		this->del(request);
	else
	{
		this->setStatus(405);
		this->serveErrorPage(errorPages);
	}
}

void Response::serveFile(std::string url, std::map<int, std::string> &errorPages, Request const &request)
{
	if (this->endWith(url, ".py") || this->endWith(url, ".php"))
		this->serveCGI(url, request);
	else
		this->serveStaticFile(url, errorPages);
}

void Response::serveDirectory(std::string directoryPath, std::map<int, std::string> &errorPages, Location const &location)
{
	std::cout << "Serving directory: " << directoryPath << std::endl;
	std::vector<std::string> indexes = location.getIndex();
	if (!indexes.empty())
	{
		size_t i = 0;
		for (std::vector<std::string>::iterator it = indexes.begin(); it != indexes.end(); ++it)
		{
			std::string index = directoryPath + "/" + *it;
			std::ifstream file(index.c_str());
			if (this->isFileExists(index))
			{
				this->serveStaticFile(index, errorPages);
				return;
			}
			i++;
		}
		if (i == indexes.size())
		{
			std::cout << "here" << std::endl;
			this->setStatus(404);
			this->serveErrorPage(errorPages);
		}
	}
	else if (location.getAutoIndex() == false)
	{
		this->setStatus(403);
		this->serveErrorPage(errorPages);
	}
	else
		this->serveDirectoryAutoIndex(directoryPath, errorPages);
}

void Response::get(const Request &request)
{
	Location const &location = request.getLocation();
	std::map<int, std::string> errorPages = location.getErrorPages();
	std::map<std::string, std::string> headers = request.getRequest();
	std::string path = request.getPath();
	std::cout << "Path: " << path << std::endl;
	this->setStatus(request.getStatus());

	std::string pathType = this->getPathType(path);

	std::cout << "pathType: " << pathType << std::endl;
	if (this->_status == 200)
	{
		if (pathType == "file")
		{
			this->setHeader("Content-Type", headers["Content-Type"]);
			this->serveFile(path, errorPages, request);
		}
		else if (pathType == "directory")
		{
			if (headers["URL"][headers["URL"].length() - 1] != '/')
				this->redirect(headers["URL"] + "/");
			else
				this->serveDirectory(path, errorPages, location);
		}
		else
		{
			this->setStatus(404);
			this->serveErrorPage(errorPages);
		}
	}
	else
		this->serveErrorPage(errorPages);
}

bool Response::is_file(const char *path)
{
	struct stat buf;
	stat(path, &buf);
	return S_ISREG(buf.st_mode);
}

bool Response::is_directory(const char *path)
{
	struct stat buf;
	stat(path, &buf);
	return S_ISDIR(buf.st_mode);
}

std::string Response::getPathType(std::string path)
{
	if (this->is_file(path.c_str()))
		return "file";
	else if (this->is_directory(path.c_str()))
		return "directory";
	else
		return "error";
}

void Response::redirect(std::string uri)
{
	std::cout << "redirect made" << std::endl;
	this->setStatus(301);
	this->setHeader("Location", uri);
}

void Response::serveDirectoryAutoIndex(std::string url, std::map<int, std::string> &errorPages)
{
	std::cout << "Serving directory: " << url << std::endl;
	// Open the directory
	DIR *dir = opendir(url.c_str());
	if (dir == NULL)
	{
		std::cout << "Error opening directory" << std::endl;
		this->setStatus(403);
		this->serveErrorPage(errorPages);
	}

	std::string directoryContent;

	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		std::string entryName = entry->d_name;
		directoryContent += "<li><a href=\"" + entryName + "\">" + entryName + "</a></li>";
	}

	closedir(dir);

	std::string responseBody = "<html><body><h1>Directory Listing</h1><ul>" + directoryContent + "</ul></body></html>";

	this->setHeader("Content-Type", "text/html");
	this->setHeader("Content-Length", to_string(responseBody.length()));
	this->setBody(responseBody);
}

void Response::serveStaticFile(std::string url, std::map<int, std::string> &errorPages)
{
	std::cout << "Serving static file: " << url << std::endl;
	std::ifstream file(url.c_str());
	if (file.is_open())
	{
		std::string responseBody((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		if (this->_status != 200)
			this->setHeader("Content-Type", "text/html");
		this->setHeader("Content-Length", to_string(responseBody.length()));
		this->setBody(responseBody);
	}
	else
	{
		std::cout << "Error opening file" << std::endl;
		this->setStatus(403);
		this->serveErrorPage(errorPages);
	}
}

bool Response::endWith(std::string const &value, std::string const &ending)
{
	if (ending.size() > value.size())
		return false;
	for (int i = 0; i < (int)ending.size(); i++)
	{
		if (ending[ending.size() - i - 1] != value[value.size() - i - 1])
			return false;
	}
	return true;
}

char **Response::getENV(std::string url, const Request &request)
{
	std::map<std::string, std::string> env;
	std::map<std::string, std::string> headers = request.getRequest();

	env["CONTENT_TYPE"] = headers["Content-Type"];
	env["CONTENT_LENGTH"] = headers["Content-Length"];
	env["REQUEST_METHOD"] = headers["Method"];
	env["REQUEST_URI"] = headers["URL"];
	env["QUERY_STRING"] = headers["Query"];
	env["REDIRECT_STATUS"] = "CGI";
	// env["SERVER_SOFTWARE"] = "webserv";
	env["SCRIPT_FILENAME"] = url;
	env["SCRIPT_NAME"] = url;

	this->addHTTPToEnvForCGI(env, headers);
	// from map to char**
	char **envp = new char *[env.size() + 1];
	int i = 0;
	for (std::map<std::string, std::string>::iterator it = env.begin(); it != env.end(); ++it)
	{
		std::string envVar = it->first + "=" + it->second;
		envp[i] = new char[envVar.length() + 1];
		std::strcpy(envp[i], envVar.c_str());

		i++;
	}
	envp[i] = NULL;
	return envp;
}

void Response::addHTTPToEnvForCGI(std::map<std::string, std::string> &env, std::map<std::string, std::string> &headers)
{
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
	{
		std::string upperKey = this->toUpperCase(it->first);
		if (env.find(upperKey) == env.end())
		{
			std::string envVar = "HTTP_" + upperKey;
			env[envVar] = it->second;
		}
	}
}

void Response::serveCGI(std::string url, const Request &request)
{
	std::cout << "Serving CGI: " << url << "is in process " << this->_isCGIInProcess << " is execution finished " << this->_isCGIFinished << std::endl;

	std::string cgiPath = url;
	Location const &location = request.getLocation();
	std::map<int, std::string> errorPages = location.getErrorPages();
	std::string extension = this->getExtention(cgiPath);
	std::map<std::string, std::string> cgi = location.getCgi();
	std::string binary = cgi[extension];
	if (binary.empty())
	{
		std::cout << "No CGI binary found for extension: " << extension << std::endl;
		this->setStatus(403);
		this->serveErrorPage(errorPages);
		return;
	}
	if (!this->_isCGIInProcess)
	{
		char **envp = this->getENV(cgiPath, request);

		this->executeCGI(cgiPath, binary, envp, errorPages);
		this->_isCGIFinished = this->checkCGIStatus(errorPages);
	}
	else if (!this->_isCGIFinished)
		this->_isCGIFinished = this->checkCGIStatus(errorPages);
	else
	{
		std::cout << "CGI execution finished" << std::endl;
		this->serveCGIFile("cgi_output.txt", errorPages);
	}
}

void Response::executeCGI(std::string cgiPath, std::string binary, char **envp, std::map<int, std::string> &errorPages)
{
	if (!this->_isCGIInProcess)
	{
		int fd[2];

		fd[0] = open(cgiPath.c_str(), O_RDONLY);
		fd[1] = open("cgi_output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
		if (fd[0] == -1 || fd[1] == -1)
		{
			std::cout << "Error opening file" << std::endl;
			this->setStatus(403);
			this->serveErrorPage(errorPages);
			return;
		}
		this->_isCGIInProcess = true;
		this->_pid = fork();
		if (this->_pid == -1)
		{
			std::cout << "Error forking" << std::endl;
			exit(1);
		}
		else if (this->_pid == 0)
		{
			dup2(fd[0], 0);
			dup2(fd[1], 1);
			close(fd[0]);
			close(fd[1]);
			if (execve(binary.c_str(), NULL, envp) == -1)
			{
				std::cout << "Error executing CGI" << std::endl;
				exit(1);
			}
		}
		else
		{
			close(fd[0]);
			close(fd[1]);
		}
	}
}

int Response::checkCGIStatus(std::map<int, std::string> &errorPages)
{
	int status;
	int w = waitpid(this->_pid, &status, WNOHANG);
	if (w == -1)
	{
		std::cout << "Error waiting for CGI" << std::endl;
		exit(1);
	}
	else if (w == 0)
	{
		std::cout << "CGI still running" << std::endl;
		return false;
	}
	else
	{
		if (WIFEXITED(status))
		{
			std::cout << "CGI exited normally" << std::endl;
			if (WEXITSTATUS(status) == 0)
			{
				std::cout << "CGI exited with status 0" << std::endl;
				return true;
			}
			else
			{
				std::cout << "CGI exited with status " << WEXITSTATUS(status) << std::endl;
				this->setStatus(403);
				this->serveErrorPage(errorPages);
				return true;
			}
		}
		else
		{
			std::cout << "CGI exited abnormally" << std::endl;
			this->setStatus(500);
			this->serveErrorPage(errorPages);
			return true;
		}
	}
}

void Response::serveCGIFile(std::string cgiPath, std::map<int, std::string> &errorPages)
{
	std::cout << "Serving CGI file" << std::endl;
	std::fstream file;
	file.open(cgiPath.c_str(), std::ios::in);
	if (!file.is_open())
	{
		std::cout << "Error opening CGI file" << std::endl;
		this->setStatus(403);
		this->serveErrorPage(errorPages);
		return;
	}
	std::string line;
	std::string header;
	std::string body;
	bool isHeader = true;
	while (getline(file, line))
	{
		if (isHeader)
		{
			if (line.empty())
			{
				isHeader = false;
				continue;
			}
			header += line + "\n";
		}
		else
		{
			body += line + "\n";
		}
	}
	this->parseResponseHeader(header);
	this->setBody(body);
	std::cout << this->toString() << std::endl;
	file.close();
}

void Response::parseResponseHeader(std::string responseHeader)
{
	std::vector<std::string> lines = this->split(responseHeader, "\r\n");
	std::vector<std::string> firstLine = this->split(lines[0], ": ");
	// this->setProtocol(firstLine[0]);
	this->setStatus(atoi(firstLine[1].c_str()));
	for (int i = 1; i < (int)lines.size(); i++)
	{
		std::vector<std::string> header = this->split(lines[i], ": ");
		this->setHeader(header[0], header[1]);
	}
}

std::vector<std::string> Response::split(const std::string &s, std::string delimiter)
{
	std::vector<std::string> parts;
	std::string::size_type start = 0;
	std::string::size_type end = s.find(delimiter);

	while (end != std::string::npos)
	{
		parts.push_back(s.substr(start, end - start));
		start = end + delimiter.length();
		end = s.find(delimiter, start);
	}

	parts.push_back(s.substr(start));

	return parts;
}

std::string Response::getExtention(std::string url)
{
	std::string extention = "";
	for (int i = url.length() - 1; i >= 0; i--)
	{
		extention = url[i] + extention;
		if (url[i] == '.')
			return extention;
	}
	return extention;
}

std::string Response::toUpperCase(std::string str)
{
	std::string upperStr = "";
	for (int i = 0; i < (int)str.length(); i++)
		upperStr += toupper(str[i]);
	return upperStr;
}

bool Response::isFileExists(const std::string &name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

void Response::serveErrorPage(std::map<int, std::string> errorPages)
{
	if (errorPages.find(this->_status) != errorPages.end() && errorPages[this->_status] != "" && this->isFileExists(errorPages[this->_status]))
		this->serveStaticFile(errorPages[this->_status], errorPages);
	else
	{
		this->serveStaticFile("static/error/" + this->intToString(this->_status) + ".html", errorPages);
	}
}