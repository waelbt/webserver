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

int Request::state = 0;

Request::Request() : _request(), _status(200)
{
}

Request::Request(std::string const &request) : _request(), _status(200)
{
    parseRequest(request);
}

Request::~Request()
{
}

void Request::getContentType(std::string const & content)
{
    if (!state)
    {
        for (int i = 0; i < 76; i++)
            cnt.insert(std::make_pair(mimeType[i], contentType[i]));
        state = 1;
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
    _request.insert(std::make_pair("Content-Type", "application/octet-stream"));
    return ;
    dotfound:
    std::string extention = content.substr(dot);
    RequestMap::iterator it = cnt.find(extention);
    _request.insert(std::make_pair("Content-Type", it->second));
}

void Request::parseUrl(std::string const &line)
{
    size_t url = line.find("/");
    size_t protocol = line.find("HTTP");
    std::string contentType = line.substr(url, protocol - 5);
    getContentType(contentType);
    _request.insert(std::make_pair("Method", line.substr(0, url - 1)));
    _request.insert(std::make_pair("URL", contentType));
    _request.insert(std::make_pair("Protocol", line.substr(protocol)));
}

void Request::parseRequest(std::string const &request)
{
    std::istringstream req(request);
    std::string line;

    std::getline(req, line);
    parseUrl(line);
    while (std::getline(req, line) && line != "\r\n")
    {
        size_t separator = line.find(": ");
        if (separator != std::string::npos){
            _request.insert(std::make_pair(line.substr(0, separator),
                                                    line.substr(separator + 2)));
        std::cout  << line  << std::endl;
        }
    }
    std::getline(req, line);
    if (line != "\0")
        _request.insert(std::make_pair("body", line));
    badFormat();
    std::cout << _status << std::endl;
}

struct invalidUrl
{
    bool operator()(const char& c)
    {
        static std::string Error(":/?#[]@!$&'()*+,=;");
        return (Error.find(std::string(1, c)) != std::string::npos);
    }
};

void Request::badFormat()
{
    // RequestMap::iterator it = _request.find("Transfer-Encoding");
    // if (it != _request.end() && it->second != "chunked") {
    //     std::cout << it->second  << std::endl;
    //     _status = 501;
    //     return;
    // }
    if (_request.find("Content-Length") == _request.end() && _request.find("Method")->second == "Post") {
        _status = 400;
        return;
    }
    // std::string url = _request.find("URL")->second;
    std::string url = "just a test";
    if ((std::find_if(url.begin(), url.end(), invalidUrl()) != url.end()))
    {
        _status = 400;
        return;
    }
    if (_request.find("URL")->second.length() > 2048){
        _status = 414;
        return;
    }
    if (_request.find("body")->second.length() > 2048){
        _status = 414;
        return;
    }
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
