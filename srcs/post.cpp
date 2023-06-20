#include "../includes/response.hpp"

void Response::post(const Request &request)
{
    Location const &location = request.getLocation();
    std::map<int, std::string> errorPages = location.getErrorPages();
	std::map<std::string, std::string> headers = request.getRequest();
    std::string bodyPath = request.getBody();

    this->setStatus(request.getStatus());
    if (this->_status == 200)
    {
        if (location.getUpload().empty())
        {
			std::cout << "No upload path -->" << bodyPath << std::endl;
			this->setStatus(201);
			this->_isHeaderParsed = true;
			this->_isRedirect = true;
			this->_isBodySent = true;
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