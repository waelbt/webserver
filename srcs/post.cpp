#include "../includes/response.hpp"

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
			    this->serveFile(path, errorPages, request);
            }
            else if (pathType == "directory")
            {
            }
            else
            {
                this->setStatus(404);
			    this->serveErrorPage(errorPages);
            }
        }
    }
    else
        this->serveErrorPage(errorPages);
}