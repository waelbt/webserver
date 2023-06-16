#include "../includes/response.hpp"

void Response::del(const Request &request)
{
    Location const &location = request.getLocation();
	std::map<int, std::string> errorPages = location.getErrorPages();
	std::map<std::string, std::string> headers = request.getRequest();
	std::string path = request.getPath();
	std::cout << "Path: " << path << std::endl;
	this->setStatus(request.getStatus());

	std::string pathType = this->getPathType(path);

	std::cout << "pathType: " << pathType << std::endl;
	if (this->_status == 200)
	{
		if (pathType == "directory")
		{
			if (headers["URL"][headers["URL"].length() - 1] != '/')
            {
				this->setStatus(409);
			    this->serveErrorPage(errorPages);
            }
			else
            {
                // if (location.getCgi().empty())
                    // run cgi on requested file with DELTE REQUEST_METHOD
                // else
                this->deleteDirectoryContent(path, errorPages);
            }
		}
        else if (pathType == "file")
		{
			this->setHeader("Content-Type", headers["Content-Type"]);
			this->serveFile(path, errorPages, request);
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


// #include <iostream>
// #include <filesystem>

// namespace fs = std::filesystem;

// void deleteFolderContents(const std::string& folderPath) {
//     for (const auto& entry : fs::directory_iterator(folderPath)) {
//         if (fs::is_directory(entry)) {
//             deleteFolderContents(entry.path().string()); // Recursive call for subdirectories
//             fs::remove(entry.path()); // Remove the empty directory
//         } else {
//             fs::remove(entry.path()); // Remove files
//         }
//     }
// }

// int main() {
//     std::string folderPath = "path_to_your_folder";
//     deleteFolderContents(folderPath);
    
//     std::cout << "Folder contents deleted successfully!" << std::endl;
//     return 0;
// }

// #include <iostream>
// #include <dirent.h>
// #include <string>

// void deleteFolderContents(const std::string& folderPath) {
//     DIR* dir = opendir(folderPath.c_str());
//     if (dir != nullptr) {
//         dirent* entry;
//         while ((entry = readdir(dir)) != nullptr) {
//             std::string entryName = entry->d_name;
//             if (entryName != "." && entryName != "..") {
//                 std::string fullPath = folderPath + "/" + entryName;
//                 if (entry->d_type == DT_DIR) {
//                     deleteFolderContents(fullPath); // Recursive call for subdirectories
//                     rmdir(fullPath.c_str()); // Remove the empty directory
//                 } else {
//                     remove(fullPath.c_str()); // Remove files
//                 }
//             }
//         }
//         closedir(dir);
//     }
// }

// int main() {
//     std::string folderPath = "path_to_your_folder";
//     deleteFolderContents(folderPath);
    
//     std::cout << "Folder contents deleted successfully!" << std::endl;
//     return 0;
// }