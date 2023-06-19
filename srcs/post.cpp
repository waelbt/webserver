#include "../includes/response.hpp"

void Response::servePostFile(std::string url, std::map<int, std::string> &errorPages, Request const &request)
{
    if (this->endWith(url, ".py") || this->endWith(url, ".php"))
		this->serveCGI(url, request);
	else
	{
		this->setStatus(403);
		this->serveErrorPage(errorPages);
	}
}

void Response::servePostDirectory(std::string directoryPath, std::map<int, std::string> &errorPages, Location const &location, Request const &request)
{
	std::cout << "Serving directory: " << directoryPath << std::endl;
	std::vector<std::string> indexes = location.getIndex();
	if (!indexes.empty())
	{
		for (std::vector<std::string>::iterator it = indexes.begin(); it != indexes.end(); ++it)
		{
			std::string index = directoryPath + "/" + *it;
			std::ifstream file(index.c_str());
			if (this->isFileExists(index))
			{
				this->servePostFile(index, errorPages, request);
				return;
			}
		}
	}
	this->setStatus(403);
	this->serveErrorPage(errorPages);
}

void Response::post(const Request &request)
{
    Location const &location = request.getLocation();
    std::map<int, std::string> errorPages = location.getErrorPages();
	std::map<std::string, std::string> headers = request.getRequest();
    std::string path = request.getPath();

    this->setStatus(request.getStatus());
    std::string pathType = this->getPathType(path);
    if (this->_status == 200)
    {
        if (location.getUpload().empty())
        {
            if (pathType == "file")
            {
                this->setHeader("Content-Type", headers["Content-Type"]);
			    this->servePostFile(path, errorPages, request);
            }
            else if (pathType == "directory")
            {
                if (headers["URL"][headers["URL"].length() - 1] != '/')
				    this->redirect(headers["URL"] + "/");
			    else
				    this->servePostDirectory(path, errorPages, location, request);
            }
            else
            {
                this->setStatus(404);
			    this->serveErrorPage(errorPages);
            }
        }
		else
		{
			this->setStatus(201);
			this->_isHeaderParsed = true;
			this->_isRedirect = true;
			this->_isBodySent = true;
		}
    }
    else
        this->serveErrorPage(errorPages);
}