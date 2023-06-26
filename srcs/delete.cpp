#include "../includes/response.hpp"

void deleteContent(const std::string& path)
{
	if (!access(path.c_str(), W_OK | X_OK))
	{
		std::string entryName;
    	DIR* dir = opendir(path.c_str());

    	if (dir != NULL) {
    	    dirent* entry;
    	    while ((entry = readdir(dir)) != NULL)
			{
				entryName = entry->d_name;
    	        if (entryName != "." && entryName != "..")
				{
    	            std::string full_path = path + "/" + entryName;
    	            if (entry->d_type == DT_DIR)
						deleteContent(full_path);
					else
					{
						if (access(full_path.c_str(), F_OK | W_OK) != 0)
							throw (-1);
						remove(full_path.c_str());
					}
    	        }
    	    }
    	    closedir(dir);
    	}
		if (rmdir(path.c_str()) == -1)
			throw (-1);
	}
	else
		throw -1;
}

void Response::delete_directory(std::string path, const Request &request)
{
	std::map<std::string, std::string> headers(request.getRequest());

	if (headers["URL"][headers["URL"].length() - 1] != '/')
		this->setStatus(409);
	else
	{
		this->setStatus(204);
		try {
			deleteContent(path); }
		catch(const int& e) {
			(!access(path.c_str(), W_OK | X_OK)) ? this->setStatus(500) : this->setStatus(403); }
	}
}

void Response::delete_file(std::string path)
{
	if (access(path.c_str(), X_OK | W_OK) != 0)
		this->setStatus(403);
	else
	{
		this->setStatus(204);
		remove(path.c_str());
	}
}

void Response::del(const Request &request)
{
	std::string path = request.getPath();

	std::cout << "Method :       DELETE\n" << std::endl;
	if (this->_isDeleted == false)
	{
		(is_directory(path.c_str())) ? delete_directory(path, request) : \
		(is_file(path.c_str())) ? delete_file(path) : this->setStatus(404);
		if (this->_status == 204)
		{
			this->_isHeaderParsed = true;
			this->_isBodySent = true;
		}
		this->_isDeleted = true;
	}
	if (this->_status != 204)
		this->serveErrorPage(request.getLocation().getErrorPages());
}
