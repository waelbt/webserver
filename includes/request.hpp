#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <utility>
#include "configuration.hpp"

#define RequestMap std::map<std::string, std::string>


struct invalidUrl
{
    bool operator()(const char& c);
};

class Request
{
    private:
        RequestMap    _request;
        Configuration _conf;
        Location      _location;
        std::string   _path;
        int           _status;
        static int    state;

        void parseUrl(std::string const &line);
        void setContentType(std::string const & content);
        void badFormat();
        void checkLocation();
        void checkMethod();
    public:
        Request();
        Request(std::string const &request, Configuration const & conf);
        ~Request();
        Request& operator=(const Request& other);

        RequestMap const      &getRequest() const;
        std::string const &   getType() const;
        int const &           getStatus() const;
        std::string const &   getPath() const;
        void                  parseRequest(std::string const &request, Configuration const & conf);
        void                  printElement();
};