#include "../includes/response.hpp"

std::map<int, std::string> _httpResponses;

Response::Response()
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

// void Response::serveResponse(const Request &request)
// {
// 	this->setStatus(request.getStatus());
// 	if (this->_status  == 200)
// 	{
// 		std::map<std::string, std::string> headers = request.getRequest();
// 		std::string path = request.getPath();
// 		this->setHeader("Content-Type", headers["Content-Type"]);
// 		this->serveFile(path);
// 	}
// 	else
// 		this->serveFile("static/error/" + this->intToString(this->_status) + ".html");
// }

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
			this->serveStaticFile(path, errorPages);
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

char **Response::getENV(const Request &request)
{
	std::map<std::string, std::string> env;
	std::map<std::string, std::string> headers = request.getRequest();

	env["CONTENT_TYPE"] = headers["Content-Type"];
	env["CONTENT_LENGTH"] = headers["Content-Length"];
	env["REQUEST_METHOD"] = headers["Method"];
	env["REQUEST_URI"] = headers["URL"];
	env["QUERY_STRING"] = headers["Query"];
	env["REDIRECT_STATUS"] = "200";
	env["SERVER_SOFTWARE"] = "webserv";
	env["HTTP_COOKIE"] = headers["Cookie"];
	env["HTTP_HOST"] = headers["Host"];
	env["SCRIPT_NAME"] = headers["SCRIPT_FILENAME"];

	// from map to char**
	char **envp = new char *[env.size() + 1];
	int i = 0;
	for (std::map<std::string, std::string>::iterator it = env.begin(); it != env.end(); ++it)
	{
		std::string envVar = it->first + "=" + it->second;
		envp[i] = new char[envVar.length() + 1];
		strcpy(envp[i], envVar.c_str());
		i++;
	}
	return envp;
}

void Response::serveCGI(std::string url, const Request &request)
{
	char **envp = this->getENV(request);
	Location location = request.getLocation();
	std::string cgiPath = url;

	(void)cgiPath;
	(void)envp;
	std::map<std::string, std::string> cgi = location.getCgi();
	for (std::map<std::string, std::string>::iterator it = cgi.begin(); it != cgi.end(); ++it)
	{
		std::string cgiKey = it->first;
		std::string cgiValue = it->second;
		std::cout << "cgiKey: " << cgiKey << std::endl;
		std::cout << "cgiValue: " << cgiValue << std::endl;
	}
}

bool Response::isFileExists(const std::string &name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

void Response::serveErrorPage(std::map<int, std::string> &errorPages)
{
	if (errorPages.find(this->_status) != errorPages.end() && errorPages[this->_status] != "" && this->isFileExists(errorPages[this->_status]))
		this->serveStaticFile(errorPages[this->_status], errorPages);
	else
		this->serveStaticFile("static/error/" + this->intToString(this->_status) + ".html", errorPages);
}