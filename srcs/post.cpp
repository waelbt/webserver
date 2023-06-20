#include "../includes/response.hpp"

void Response::post(const Request &request)
{
    Location const &location = request.getLocation();
    std::map<int, std::string> errorPages = location.getErrorPages();
	std::map<std::string, std::string> headers = request.getRequest();
	std::string path = request.getPath();
	std::string pathType = this->getPathType(path);

    this->setStatus(request.getStatus());
    if (this->_status == 200)
    {
        // if (location.getUpload().empty())
        // {
		// 	this->serveCGI(path, request);
		// 	this->setStatus(201);
		// 	this->_isHeaderParsed = true;
		// 	this->_isRedirect = true;
		// 	this->_isBodySent = true;
        // }
		// else
		// {
		// 	this->serveCGI(path, request);
		// 	this->setStatus(201);
		// 	this->_isHeaderParsed = true;
		// 	this->_isRedirect = true;
		// 	this->_isBodySent = true;
		// }
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