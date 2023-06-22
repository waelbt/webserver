#include "../includes/response.hpp"

int deleteContent(const std::string& path)
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
	}
	else
		return -2;
	return 0;
}

void Response::del(const Request &request)
{
	std::map<std::string, std::string> headers(request.getRequest());
	std::string path = request.getPath();
	int stat;

	if (is_directory(path.c_str()))
	{
		if (headers["URL"][headers["URL"].length() - 1] != '/')
			this->setStatus(409);
		else
		{
			stat = deleteContent(path);
			if (stat == -1)
			{
				if (!access(path.c_str(), W_OK | X_OK))
					this->setStatus(500);
				else
					this->setStatus(403);
			}
			else if (stat == -2)
				this->setStatus(409);
			else
				this->setStatus(204);
		}
	}
    else if (is_file(path.c_str())) 
	{
		if (remove(path.c_str()) == -1)
			this->setStatus(403);
		else
    		this->setStatus(204);
	}
	else
		this->setStatus(404);
}
