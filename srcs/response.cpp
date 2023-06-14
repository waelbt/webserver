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

void Response::sendResponse(int clientSocket)
{
	std::string response = this->toString();
	std::cout << "Response: " << std::endl
			  << response << std::endl;
	send(clientSocket, response.c_str(), response.length(), 0);
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

void Response::serveFile(std::string url, std::map<int, std::string> &errorPages)
{
	std::string filename = url;
	std::ifstream file(filename.c_str());
	if (file.is_open())
	{
		std::cout << "Serving file: " << filename << std::endl;
		std::string line;
		std::string body;
		while (getline(file, line))
		{
			body += line + "\n";
		}
		file.close();
		this->setBody(body);
		if (this->_status != 200)
			this->setHeader("Content-Type", "text/html");
		this->setHeader("Content-Length", std::to_string(body.length()));
	}
	else
	{
		std::cout << "Error opening file: " << filename << std::endl;
		this->setStatus(404);
		std::ifstream file(errorPages[this->_status].c_str());
		if (errorPages.find(this->_status) != errorPages.end() && errorPages[this->_status] != "" && file.is_open())
			this->serveFile(errorPages[this->_status], errorPages);
		else
			this->serveFile("static/error/" + this->intToString(this->_status) + ".html", errorPages);
	}
}

void Response::serveDirectory(std::string directoryPath, std::map<int, std::string> &errorPages, Location const & location)
{
	std::cout << "Serving directory: " << directoryPath << std::endl;
	std::vector<std::string> indexes = location.getIndex();
	if (!indexes.empty())
	{
		int i = 0;
		for (std::vector<std::string>::iterator it = indexes.begin(); it != indexes.end(); ++it)
		{
			i++;
			std::string index = directoryPath + *it;
			std::ifstream file(index.c_str());
			if (file.is_open())
			{
				this->serveFile(index, errorPages);
				break;
			}
		}
		if (i == indexes.size())
		{
			this->setStatus(404);
			this->serveFile(errorPages[this->_status], errorPages);
		}
	}
	else if (location.getAutoIndex() == false)
	{
		this->setStatus(403);
		this->serveFile(errorPages[this->_status], errorPages);
	}
	else
		this->serveDirectoryAutoIndex(directoryPath, errorPages);
}

void Response::get(const Request &request)
{
	Location const & location = request.getLocation();
	std::map<int, std::string> error_pages = location.getErrorPages();
	std::map<std::string, std::string> headers = request.getRequest();
	std::string path = request.getPath();

	this->setStatus(request.getStatus());

	std::string pathType = this->getPathType(path);

	if (this->_status == 200 && pathType != "error")
	{
		if (pathType == "file")
		{
			this->setHeader("Content-Type", headers["Content-Type"]);
			this->serveFile(path, error_pages);
		}
		else if (pathType == "directory")
		{
			if (headers["URL"][headers["URL"].length() - 1] != '/')
				this->setHeader("Location", headers["URL"] + "/");
			else
				this->serveDirectory(path, error_pages, location);
		}
	}
	else
		this->serveFile(error_pages[this->_status], error_pages);
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
	this->setHeader("Location", uri);
	this->setHeader("Content-Type", "text/html");
	this->setHeader("Content-Length", "0");
	this->setBody("");
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
		this->serveFile(errorPages[this->_status], errorPages);
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
	this->setHeader("Content-Length", std::to_string(responseBody.length()));
	this->setBody(responseBody);
}