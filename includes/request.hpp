#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <utility>
#include <math.h>
#include <fstream>
#include "configuration.hpp"

#define RequestMap std::map<std::string, std::string>


struct invalidUrl
{
    bool operator()(const char& c);
};

enum ChunkState
{
    DONE,
    UNDONE,
};

class Request
{
    private:
        RequestMap    _request;
        Configuration _conf;
        Location      _location;
        std::string   _path;
        std::string   _body;
        ChunkState    _chunkState;
        size_t        _chunkSize;
        int           _status;
        size_t         _bodySize;
        static int    contentState;

        void parseUrl(std::string const &line);
        void setContentType(std::string const & content);
        void badFormat();
        void checkLocation();
        void checkMethod();
        void setBody(std::istringstream &req);
        void setFullBody(std::istringstream &req);
        void setChunkedBody(std::istringstream &req);
    public:
        Request();
        ~Request();
        Request& operator=(const Request& other);

        RequestMap const      &getRequest() const;
        std::string const &   getType() const;
        int const &           getStatus() const;
        std::string const &   getPath() const;
        Location const &      getLocation() const;
        ChunkState const &    getChunkedState() const;
        void                  parseRequest(std::string const &request, Configuration const & conf, int &r);
        void                  printElement();
};