#include <iostream>
#include <string>
#include <exception>
#include <algorithm>
#include <fstream>
#include <stack>
#include <stdlib.h>
#include <map>
#include <sstream>
#include <fcntl.h>
#include <queue>
#include <string>
#include <exception>
#include <vector>
#include <sys/types.h>
#include <signal.h>
#include <libgen.h>
 #include <sys/stat.h>
#include <unistd.h>

int stat_checkout(std::string path)
{
	struct stat buf;

	if(!access(path.c_str(), F_OK))
	{
        if (stat(path.c_str(), &buf) == -1)
            return 403;
	}
	return 404;
}

int main()
{
    struct stat buf ;
    std::string path = "delete/test.html";

    std::cout << stat_checkout(path) << std::endl;
}