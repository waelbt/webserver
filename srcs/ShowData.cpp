#include "../includes/server.hpp"

std::ostream& operator<<(std::ostream& o, s_err_pages obj)
{
    for (std::vector<int>::iterator it = obj._status.begin(); it != obj._status.end(); it++)
        std::cout << " " << *it;
    std::cout << " " << obj._page << ";" << std::endl;
    return o;
}


std::ostream& operator<<(std::ostream& o, s_cgi obj)
{
    for (std::vector<std::string>::iterator it = obj._exec.begin(); it != obj._exec.end(); it++)
        std::cout << " " << *it;
    std::cout << " " << obj._path << ";" <<std::endl;
    return o;
}



std::ostream& operator<<(std::ostream& o, Route obj)
{
    std::cout << "location  " << obj._pattren  << ": " << std::endl;
    std::cout << "  hosts: "<< obj._host << std::endl;
    std::cout << "  listen:";
    print_vec(obj._ports, "");
    std::cout << ";" << std::endl;
    std::cout << "  server name:";
    print_vec(obj._server_name, "");
    std::cout << "  root: " << obj._root << ";" << std::endl;
    std::cout << "  index: ";
    print_vec(obj._index, "");
    std::cout << ";" << std::endl;
    print_vec(obj._error_pages, "  error pages:");
    std::cout << "  client_max_body_size: " << obj._client_max_body_size << ";" << std::endl;
    std::cout << "  auto index: " << ((obj._AutoIndex) ? (std::cout << "true") :  (std::cout << "false")) << ";" << std::endl;
    std::cout << "  upload: " << obj._upload << ";" << std::endl;
    std::cout << "  limit_except: ";
    print_vec(obj._limit_except, "");
    std::cout << ";" << std::endl;
    print_vec(obj._cgi, "  cgi:");
    std::cout << std::endl;
    print_vec(obj._redirect, "  redirect:");
    return o;
}
