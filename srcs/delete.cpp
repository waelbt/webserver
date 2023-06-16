#include "../includes/response.hpp"

int deleteContent(const std::string& path)
{
	std::string entryName;
    DIR* dir = opendir(path.c_str());

    if (dir != nullptr) {
        dirent* entry;
        while ((entry = readdir(dir)) != nullptr)
		{
			entryName = entry->d_name;
            if (entryName != "." && entryName != "..")
			{
                std::string full_path = path + "/" + entryName;
                if (entry->d_type == DT_DIR)
				{
                    deleteContent(full_path);
					if (rmdir(full_path.c_str()) == -1)
						return (-1);
                }
				else
				{
					if (remove(full_path.c_str()) == -1)
						return (-1);
				}
            }
        }
        closedir(dir);
    }
	return 0;
}

void Response::del(const Request &request)
{
	std::map<std::string, std::string> headers(request.getRequest());
	std::string path = request.getPath();

	this->setStatus(request.getStatus());
	if (this->_status == 200)
	{
		if (is_directory(path.c_str()))
		{
			if (headers["URL"][headers["URL"].length() - 1] != '/')
				this->setStatus(409);
			else
				(deleteContent(path) == -1) ? ((!access(path.c_str(), W_OK)) ? \
				this->setStatus(500) : this->setStatus(403)) : this->setStatus(204);
		}
        else if (is_file(path.c_str())) {
            unlink(path.c_str()); this->setStatus(204);}
		else
			this->setStatus(404);
	}
	this->serveErrorPage((request.getLocation()).getErrorPages());
}
