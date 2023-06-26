#include "../includes/response.hpp"

std::map<int, std::string> _httpResponses;

void Response::initHTTPResponses()
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


Response::Response(): _status(200), _length(0), _isCGIInProcess(0), _isCGIFinished(0), _isCGIParsed(0), _isFileOpned(0), _isHeaderSent(0), _isBodySent(0), _isHeaderParsed(0), _isRedirect(0), _isDeleted(false) , _body(""), _generatedName(""), _cgiOutput(""), _headers()
{
	this->initHTTPResponses();
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

void Response::redirectLocation(const Request &request, std::string url)
{
	if (url[0] != '/')
		url = "/" + url;
	Configuration conf = request.getConf();
	std::vector<Location> locations = conf.getLocations();
    std::vector<Location>::iterator it = locations.begin();
	Location newLocation;
    std::string upper;

    for(;it != locations.end();it++)
    {
        std::string pattern = (*it).getPattren();
        if (url.length() <  pattern.length())
            continue ;
        std::string lower = url.substr(0, pattern.length());
        if (pattern == lower && (url[pattern.length()] == '\0' || url[pattern.length()] == '/'))
        {
            if (upper.empty())
            {
                upper = lower;
                newLocation = *it;
            }
            else
            {
                if (lower.length() > upper.length())
                {
                    upper = lower;
                    newLocation = *it;
                }
            }
        }
    }
    if (upper.empty())
	{
		this->setStatus(404);
		this->serveErrorPage(newLocation.getErrorPages());
	}
	else
		this->redirect(upper);
}

void Response::serveResponse(const Request &request)
{
	Location location = request.getLocation();
	std::string method = request.getRequest().find("Method")->second;
	std::map<int, std::string> errorPages = location.getErrorPages();

	if (!this->_isDeleted)
		this->setStatus(request.getStatus());

	if (this->_status != 200)
		this->serveErrorPage(errorPages);
	else if (!location.getRedirect().empty())
		this->redirectLocation(request, location.getRedirect());
	else if (method == "GET")
		this->get(request);
	else if (method == "POST")
		this->post(request);
	else if (method == "DELETE")
		this->del(request);
}

void Response::serveFile(std::string url, std::map<int, std::string> &errorPages, Request const &request)
{
	std::string extention = this->getExtention(url);
	std::map<std::string, std::string> cgi = request.getLocation().getCgi();

	if (cgi.find(extention) != cgi.end())
		this->serveCGI(url, request);
	else
		this->serveStaticFile(url, errorPages);
}

void Response::serveDirectory(std::string directoryPath, std::map<int, std::string> &errorPages, Location const &location, const Request &request)
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
				this->serveFile(index, errorPages, request);
				return;
			}
			i++;
		}
		if (i == indexes.size())
		{
			if (location.getAutoIndex() == true)
				this->serveDirectoryAutoIndex(directoryPath, errorPages);
			else
			{
				this->setStatus(403);
				this->serveErrorPage(errorPages);
			}
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

	std::cout << "Method :       GET\n" << std::endl;
	std::string pathType = this->getPathType(path);

	if (pathType == "file")
	{
		if (!this->_isFileOpned)
			this->setHeader("Content-Type", headers["Content-Type"]);
		this->serveFile(path, errorPages, request);
	}
	else if (pathType == "directory")
	{
		if (headers["URL"][headers["URL"].length() - 1] != '/')
			this->redirect(headers["URL"] + "/");
		else
			this->serveDirectory(path, errorPages, location, request);
	}
	else
	{
		this->setStatus(404);
		this->serveErrorPage(errorPages);
	}
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
	this->_isHeaderParsed = true;
	this->_isRedirect = true;
	this->_isBodySent = true;
}

void Response::serveDirectoryAutoIndex(std::string url, std::map<int, std::string> &errorPages)
{
	std::cout << "Serving directory: " << url << std::endl;
	// Open the directory
	DIR *dir = opendir(url.c_str());
	if (dir == NULL)
	{
		std::cerr << "Error opening directory" << std::endl;
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
	this->_isHeaderParsed = true;
	std::time_t result = std::time(NULL);
	this->_generatedName = "/tmp/" + this->generateRandomFile(result) + ".txt";
	std::fstream file(this->_generatedName.c_str(), std::ios::out);
	file << responseBody;
	file.close();
	this->serveStaticFile(this->_generatedName, errorPages);
}

void Response::serveStaticFile(std::string url, std::map<int, std::string> &errorPages)
{
	std::cout << "Serving static file: " << url << std::endl;
	if (!this->_isFileOpned)
	{
		this->_file.close();
		this->_file.open(url.c_str(), std::ios::in | std::ios::binary);
		if (this->_file.fail()) {
			std::cerr << "Failed to open file" << std::endl;
			this->setStatus(403);
			this->serveErrorPage(errorPages);
		}
		else
			this->setIsFileOpned(true);
	}
	if (this->_isFileOpned)
	{
		if (this->_isHeaderSent == false)
		{
			if (this->_status != 200)
				this->setHeader("Content-Type", "text/html");
			this->_file.seekg(0, std::ios::end);
			std::cout << "File size: " << this->_file.tellg() << std::endl;
			this->setHeader("Content-Length", to_string(this->_file.tellg()));
			this->_file.seekg(0, std::ios::beg);
			this->_isHeaderParsed = true;
		}
		else
		{
			char buffer[65536];
			this->_file.seekg(this->_length);
			this->_file.read(buffer, sizeof(buffer));
			this->_length = this->_file.tellg();
			std::streamsize bytesRead = this->_file.gcount();

			if (bytesRead > 0)
			{
				std::string chunk(buffer, bytesRead);
				this->setBody(chunk);
				this->_file.close();
				this->setIsFileOpned(false);
			}
			else
			{
				this->setIsBodySent(true);
				this->_file.close();
				this->setIsFileOpned(false);
			}
		}
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
	env["REQUEST_METHOD"] = headers["Method"];
	env["REQUEST_URI"] = headers["URL"];
	env["QUERY_STRING"] = request.getQueries();
	env["REDIRECT_STATUS"] = "CGI";
	// env["SERVER_SOFTWARE"] = "webserv";
	env["SCRIPT_FILENAME"] = url;
	env["SCRIPT_NAME"] = url;
	if (headers["Method"] == "POST")
	{
		env["REDIRECT_STATUS"] = "1";
		env["CONTENT_LENGTH"] = this->size_tToString(request.getContentLength());
	}

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
	// exit(0);
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
		this->executeCGI(cgiPath, binary, errorPages, request);
		this->_isCGIFinished = this->checkCGIStatus(errorPages);
	}
	else if (!this->_isCGIFinished)
		this->_isCGIFinished = this->checkCGIStatus(errorPages);
	else
	{
		std::cout << "CGI execution finished" << std::endl;
		this->serveCGIFile(this->_cgiOutput, errorPages);
	}
}

void Response::executeCGI(std::string cgiPath, std::string binary, std::map<int, std::string> &errorPages, const Request &request)
{
	if (!this->_isCGIInProcess)
	{
		std::map<std::string, std::string> headers = request.getRequest();
		int fd[2] = {0, 1};

		if (headers["Method"] == "POST") {
			std::string bodyPath = request.getBody();
			fd[0] = open(bodyPath.c_str(), O_RDONLY);
		}
		if (this->_cgiOutput.empty())
			this->_cgiOutput = "/tmp/" + this->generateRandomFile(std::time(NULL)) + ".txt";
		fd[1] = open(this->_cgiOutput.c_str() , O_WRONLY | O_CREAT | O_TRUNC, 0666);
		if (fd[0] == -1 || fd[1] == -1)
		{
			std::cerr << "Error opening file" << std::endl;
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
			char **envp = this->getENV(cgiPath, request);
			dup2(fd[0], 0);
			dup2(fd[1], 1);
			close(fd[0]);
			close(fd[1]);
			char *args[3] = {(char*)binary.c_str(), (char*)cgiPath.c_str(), NULL};
			if (execve(binary.c_str(), args, envp) == -1)
			{
				std::cerr << "Error executing CGI" << std::endl;
				exit(1);
			}
		}
		if (headers["Method"] == "POST") {
			close(fd[0]);
		}
		close(fd[1]);
	}
}

int Response::checkCGIStatus(std::map<int, std::string> &errorPages)
{
	int status;
	int w = waitpid(this->_pid, &status, WNOHANG);
	if (w == -1)
	{
		std::cout << "Error waiting for CGI" << std::endl;
		this->setStatus(500);
		this->serveErrorPage(errorPages);
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
			return true;
		}
		else
		{
			std::cout << "CGI exited abnormally" << std::endl;
			this->setStatus(500);
			this->serveErrorPage(errorPages);
			return true;
		}
	}
	return true;
}

void Response::serveCGIFile(std::string cgiPath, std::map<int, std::string> &errorPages)
{
	std::cout << "Serving CGI file" << std::endl;
	if (!this->_isCGIParsed)
	{

		if (!this->_isFileOpned)
		{
			this->_file.close();
			this->_file.open(cgiPath.c_str(), std::ios::in | std::ios::binary);
			if (this->_file.fail())
			{
				std::cerr << "Failed to open file" << std::endl;
				this->setStatus(403);
				this->serveErrorPage(errorPages);
			}
			else
				this->setIsFileOpned(true);
		}

		if (this->_isFileOpned)
		{
			std::string line;
			std::string header;

			while (getline(this->_file, line))
			{
				if (line == "\r")
					break;
				header += line;
				header += "\n";
			}

			this->parseResponseHeader(header);

			// Open the file as ofstream and write line by line to it:
			std::ofstream ofs(cgiPath.c_str(), std::ios::out | std::ios::trunc);
			while (getline(this->_file, line))
			{
				ofs << line << "\n";
				std::cout << line << std::endl;
			}
			// Close the ifstream and ofstream:
			this->_file.close();
			ofs.close();
			// exit(0);
			this->setIsFileOpned(false);
		}
		this->_isCGIParsed = true;
	}
	std::cout << "CGI parsed" << std::endl;
	this->serveStaticFile(cgiPath, errorPages);
}

void Response::parseResponseHeader(std::string responseHeader) {
    std::vector<std::string> lines = this->split(responseHeader, "\r\n");
    if (lines.empty()) return; // If lines are empty, don't proceed.

    std::vector<std::string> firstLine = this->split(lines[0], ": ");
    if (firstLine.size() < 2) return; // If we don't have at least 2 parts, don't proceed.

    if (firstLine[0] == "Status") {
        if (firstLine[1].find_first_not_of("0123456789") == std::string::npos) {
            // Check if the status code is an integer.
            this->setStatus(atoi(firstLine[1].c_str()));
        } else {
            this->setStatus(200);
        }
    } else {
        this->setHeader(firstLine[0], firstLine[1]);
        this->setStatus(200);
    }

    for (int i = 1; i < (int)lines.size(); i++) {
        std::vector<std::string> header = this->split(lines[i], ": ");
        if (header[0].empty() || header.size() < 2) continue; // If header name or value is missing, don't proceed.

        if (header[0] == "Content-type")
            header[0] = "Content-Type";
        if (this->_headers.find(header[0]) != this->_headers.end()) {
            std::string value = this->_headers[header[0]];
            if (!value.empty())
                value += " \r\n" + header[0] + ": ";
            value += header[1];
            header[1] = value;
        }
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
		this->serveStaticFile("static/error/" + this->intToString(this->_status) + ".html", errorPages);
}
std::string Response::sendHeader()
{
	std::stringstream ss;
	ss << "HTTP/1.1 " << this->intToString(this->_status) << " " << _httpResponses[this->_status] << "\r\n";
	for (std::map<std::string, std::string>::const_iterator it = this->_headers.begin(); it != this->_headers.end(); ++it)
	{
		ss << it->first << ": " << it->second << "\r\n";
	}
	ss << "\r\n";
	this->setIsHeaderSent(true);
	return ss.str();
}

void Response::setIsHeaderSent(bool isHeaderSent)
{
	this->_isHeaderSent = isHeaderSent;
}

void Response::setIsBodySent(bool isBodySent)
{
	this->_isBodySent = isBodySent;
}

void Response::setIsFileOpned(bool isFileOpen)
{
	this->_isFileOpned = isFileOpen;
}

bool Response::getIsHeaderSent() const
{
	return this->_isHeaderSent;
}

bool Response::getIsBodySent() const
{
	return this->_isBodySent;
}

bool Response::getIsHeaderParsed() const
{
	return this->_isHeaderParsed;
}

bool Response::getIsRedirect() const
{
	return this->_isRedirect;
}

std::string Response::size_tToString(size_t size)
{
	std::ostringstream oss;
	oss << size;
	return oss.str();
}

void Response::reset()
{
	this->_status = 200;
	this->_length = 0;
	this->_headers.clear();
	this->_body.clear();
	this->_isHeaderSent = false;
	this->_isBodySent = false;
	this->_isHeaderParsed = false;
	this->_isRedirect = false;
	this->_isFileOpned = false;
	this->_isCGIInProcess = false;
	this->_isCGIFinished = false;
	this->_isCGIParsed = false;
	this->_isDeleted = false;
}

std::string Response::generateRandomFile(std::time_t result)
{
	static const char charset[] = "abcdefghijklmnopqrstuvwxyz";
	const int charsetSize = sizeof(charset) - 1;

	std::string randomFile;
	srand(time(0));

	for (int i = 0; i < 10; ++i)
	{
		int randomIndex = rand() % charsetSize;
		randomFile += charset[randomIndex];
	}

	return randomFile + this->intToString(result);
}