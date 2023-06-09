#include "../includes/response.hpp"

std::map<int, std::string> _httpResponses;

Response::Response() : _body("") 
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

std::string Response::getBody() const { return this->_body; }

void Response::setBody(std::string body) { this->_body = body; }

std::string Response::getStatus() const { return this->_status; }

void Response::setStatus(std::string status) { this->_status = status; }

std::map<std::string, std::string> Response::getHeaders() const { return this->_headers; }

void Response::setHeader(std::string key, std::string value) { this->_headers[key] = value; }

std::string Response::toString() const
{
	std::stringstream ss;
	ss << "HTTP/1.1 " << this->_status << " " << _httpResponses[this->_status] << "\r\n";
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

std::string Response::serveResponse(int clientSocket, Request &request)
{
	this->setStatus(request.getStatus());
	if (this->_status  == 200)
	{
		std::string path = request.getPath();
		this->setHeader("Content-Type", request.getContentType());
		this->serveFile(path);
	}
	else
		this->serveFile("static/error/" + this->_status + ".html");
	return this->toString();
}

std::string Response::serveFile(std::string url)
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
			this->setHeaders("Content-Type", "text/html");
		this->setHeaders("Content-Length", std::to_string(body.length()));
	}
	else
	{
		std::cout << "Error opening file: " << filename << std::endl;
		this->setStatus(404);
		this->serveFile("static/error/404.html");
	}
}