#include "../includes/request.hpp"

std::string mimeType[76] = {".aac", ".abw", ".arc", ".avif", ".avi", ".azw", ".bin", ".bmp", ".bz", ".bz2", ".cda",
        ".csh", ".css", ".csv", ".doc", ".docx", ".eot", ".epub", ".gz", ".gif", ".htm", ".html", ".ico", ".ics", ".jar",
        ".jpeg", ".jpg", ".js", ".json", ".jsonld", ".mid", ".midi", ".mjs", ".mp3", ".mp4", ".mpeg", ".mpkg", ".odp",
        ".ods", ".odt", ".oga", ".ogv", ".ogx", ".opus", ".otf", ".png", ".pdf", ".php", ".ppt", ".pptx", ".rar",
        ".rtf", ".sh", ".svg", ".tar", ".tif", ".tiff", ".ts", ".ttf", ".txt", ".vsd", ".wav", ".weba", ".webm",
        ".webp", ".woff", ".woff2", ".xhtml", ".xls", ".xlsx", ".xml", ".xul", ".zib", ".3gp", ".3g2", ".7z"};

std::string contentType[76] = {"audio/aac", "application/x-abiword", "application/octet-stream", "image/avif", "video/x-msvideo",
        "application/vnd.amazon.ebook", "application/octet-stream", "image/bmp", "application/x-bzip", "application/x-bzip2",
        "application/x-cdf", "application/x-csh", "text/css", "text/csv", "application/msword", "application/vnd.openxmlformats-officedocument.wordprocessingml.document",
        "application/vnd.ms-fontobject", "application/epub+zip", "application/gzip", "image/gif", "text/html", "text/html", "image/x-icon", "text/calendar",
        "application/java-archive", "image/jpeg", "image/jpeg", "application/javascript", "application/json", "application/ld+json", "audio/midi", "audio/midi",
        "application/javascript", "audio/mpeg", "video/mp4", "video/mpeg", "application/vnd.apple.installer+xml", "application/vnd.oasis.opendocument.presentation",
        "application/vnd.oasis.opendocument.spreadsheet", "application/vnd.oasis.opendocument.text", "audio/ogg", "video/ogg", "application/ogg", "audio/opus", "font/otf",
        "image/png", "application/pdf", "application/php", "application/vnd.ms-powerpoint", "application/vnd.openxmlformats-officedocument.presentationml.presentation",
        "application/x-rar-compressed", "application/rtf", "application/x-sh", "image/svg+xml", "application/x-tar", "image/tiff", "image/tiff", "video/mp2t", "font/ttf",
        "text/plain", "application/vnd.visio", "audio/wav", "audio/webm", "video/webm", "image/webp", "font/woff", "font/woff2", "application/xhtml+xml", "application/vnd.ms-excel",
        "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", "application/xml", "application/vnd.mozilla.xul+xml", "application/x-rar-compressed", "video/3gpp",
        "video/3gpp2", "application/x-7z-compressed"};

RequestMap cnt;

int Request::contentState = 0;

bool invalidUrl::operator()(const char& c)
{
    static std::string Error(":?#[]@!$&'()*+,=;");
    return (Error.find(std::string(1, c)) != std::string::npos);
}

Request::Request() : _request(), _conf(), _location(), _path(), _chunkState(UNDONE), _status(200)
{
}

Request::Request(std::string const &request, Configuration const & conf) : _request(), _conf(conf), _location(), _path(), _chunkState(UNDONE), _status(200)
{
    parseRequest(request, conf);
}

Request::~Request()
{
}

void Request::setBody(std::string const &body)
{
    std::istringstream req(body);
    std::string line;

    std::getline(req, line);
    if (line != "\0")
        _request["body"] = line;
    badFormat(); 
    std::cout << std::endl;
}
  
void Request::checkMethod()
{
    std::string method = _request.find("Method")->second;
    std::vector<std::string> limitExcept = _location.getLimit_except();
    std::string url = _request["URL"];
    std::string pattern = _location.getPattren();

    for(std::vector<std::string>::iterator it = limitExcept.begin(); it != limitExcept.end(); it++)
    {
        if ((*it) == method)
        {
            if(url.length() != pattern.length())
                _path = _location.getRoot() + "/" + url.substr(pattern.length());
            else
            {
                std::vector<std::string> index = _location.getIndex();
                if (index.empty())
                    _status = 404;
                else
                {
                    setContentType(index[0]);
                    _path = _location.getRoot() + "/" + index[0];
                }
            }
        }
    }
    if (_path.empty() && _status != 404)
        _status = 405;
}

void Request::checkLocation()
{
    std::vector<Location> locations = _conf.getLocations();
    std::vector<Location>::iterator it = locations.begin();
    std::string upper;
    std::string url = _request["URL"];

    for(;it != locations.end();it++)
    {
        std::string pattern = (*it).getPattren();
        if ((pattern == "/" && url != "/") || url.length() <  pattern.length())
            continue ;
        std::string lower = url.substr(0, pattern.length());
        if (pattern == lower && (url[pattern.length()] == '\0' || url[pattern.length()] == '/'))
        {
            if (upper.empty())
            {
                upper = lower;
                _location = *it;
            }
            else
            {
                if (lower.length() < upper.length())
                {
                    upper = lower;
                    _location = *it;
                }
            }
        }
    }
    if (upper.empty())
    {
        it = locations.begin();
        for(;it != locations.end();it++)
        {
            if ((*it).getPattren() == "/")
            {
                _location = *it;
                checkMethod();
                return;
            }
        }
        _status = 404;
    }
    else
        checkMethod();
}

void Request::setContentType(std::string const & content)
{
    if (!contentState)
    {
        for (int i = 0; i < 76; i++)
            cnt[mimeType[i]] = contentType[i];
        contentState = 1;
    }
    size_t dot;
    for (int i = content.length() - 1; i >= 0; i--)
    {
        if (content[i] == '.')
        {
            dot = i;
            goto dotfound;
        }
    }
    _request["Content-Type"] = "text/plain";
    return ;
    dotfound:
    std::string extention = content.substr(dot);
    RequestMap::iterator it = cnt.find(extention);
    _request["Content-Type"] =  it->second;
}

void Request::parseUrl(std::string const &line)
{
    size_t url = line.find("/");
    size_t protocol = line.find("HTTP");
    std::string content = line.substr(url, protocol - 5);
    setContentType(content);
    _request["Method"] = line.substr(0 , url - 1);
    _request["URL"] = content;
    _request["Protocol"] = line.substr(protocol, line.length() - protocol - 1);
}

void Request::badFormat()
{
    RequestMap::iterator transferIt = _request.find("Transfer-Encoding");
    RequestMap::iterator bodyIt = _request.find("body");
    std::string url = _request.find("URL")->second;

    if (transferIt != _request.end() && transferIt->second != "chunked")
        _status = 501;
    else if (_request.find("Content-Length") == _request.end() && _request.find("Method")->second == "Post")
        _status = 400;
    // else if ((std::find_if(url.begin(), url.end(), invalidUrl()) != url.end()))
    //     _status = 400;
    else if (url.length() > 2048)
        _status = 414;
    else if (bodyIt != _request.end() && bodyIt->second.length() > 2048)
        _status = 413;
    if (_status == 200)
        checkLocation();
}

void Request::parseRequest(std::string const &request, Configuration const & conf)
{
    std::string line(request);
    std::istringstream req(request);

    if (!_request.empty())
        goto setbody;
    _conf = conf;
    std::getline(req, line);
    parseUrl(line);
    while (std::getline(req, line) && line != "\r\n")
    {
        size_t separator = line.find(": ");
        if (separator != std::string::npos)
            _request[line.substr(0, separator)] = line.substr(separator + 2, line.length() - separator - 3);
        std::cout << line << std::endl;
    }
    setbody:
    setBody(line);
}


void Request::printElement()
{
    int i = 0;
    for(std::map<std::string, std::string>::iterator it = _request.begin();
        it != _request.end(); it++, i++)
        std::cout << i << " " << it->first << ": " << it->second << std::endl;
}

RequestMap const & Request::getRequest() const
{
    return _request;
}

int const &   Request::getStatus() const
{
    return _status;
}

std::string const &   Request::getPath() const
{
    return _path;
}

Location const &  Request::getLocation() const
{
    return _location;
}