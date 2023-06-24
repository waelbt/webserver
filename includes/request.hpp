#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <utility>
#include <math.h>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include "Registry.hpp"
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
        char *        _header;
        std::string   _body;
        std::string   _queries;
        std::string   _extention;
        std::string   _chunkedBodySize;
        ChunkState    _chunkState;
        std::ofstream _fdBody;
        size_t        _bodySize;
        size_t        _chunkSize;
        int           _status;
        size_t        _contentLength;
        int           _badFormat;
        int           _headerDone;
        static int    contentStatePost;
        static int    contentStateGet;

        std::string decodeUrl(std::string const &url);
        std::string decipherUrl(std::string const &url);
        void parseUrl(std::string const &line);
        void fullRequest(char *request, const Registry& registry,const  ConfVec& configs, int &r);
        void setContentTypePost(std::string const & content);
        void setContentTypeGet(std::string const & content);
        void badFormat();
        void checkLocation();
        void checkMethod();
        void setBody(char *request, int &r);
        void setFullBody(char *request, int &r);
        void setChunkedBody(char *request, int &r);
        bool is_directory(const char *path);
        bool is_file(const char *path);
        void setBodyPath();
        int  readChunkedBody(char *request, int &r);
        void  config_matching(const Registry& registry,const  ConfVec& configs);
    public:
        Request();
        ~Request();
        Request& operator=(const Request& other);

        RequestMap const  &   getRequest() const;
        std::string const &   getQueries() const;
        int const &           getStatus() const;
        std::string const &   getPath() const;
        Location const &      getLocation() const;
        ChunkState const &    getChunkedState() const;
        std::string const &   getBody() const;
        size_t const &        getContentLength() const;
        Configuration const & getConf() const;
        void                  parseRequest(char *request, const Registry& registry, const  ConfVec& configs, int &r);
        void                  printElement();
        std::string           get_attribute(const std::string& key); // wael ---------
};