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

RequestMap cntPost;

RequestMap cntGet;

int Request::contentStatePost = 0;

int Request::contentStateGet = 0;

bool invalidUrl::operator()(const char& c)
{
    static std::string Error("^0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ,.-!/()=?`*;:_{}[]\\|~");
    return (Error.find(std::string(1, c)) != std::string::npos);
}

std::string generateRandomFile() {
    static const char charset[] = "abcdefghijklmnopqrstuvwxyz";
    const int charsetSize = sizeof(charset) - 1;
    
    std::string randomFile;
    srand(time(0));
    
    for (int i = 0; i < 10; ++i) {
        int randomIndex = rand() % charsetSize;
        randomFile += charset[randomIndex];
    }
    
    return randomFile;
}

size_t hexaToDecimal(std::string const &str)
{
    if (str.empty())
        return (0);
    std::string hexa = "0123456789abcdef";
    size_t decimal = 0;
    size_t power = str.length() - 1;

    for (size_t i = 0; i < str.length();i++, power--)
        decimal += (hexa.find(str[i]) * pow(16, power));
    return decimal;
}

int isHexa(char c)
{
    std::string hexa = "0123456789abcdef";

    if (hexa.find(c) != std::string::npos)
        return 1;
    return 0;
}

size_t stringToDecimal(std::string const &str)
{
    if (str.empty())
        return (0);
    std::string hexa = "0123456789";
    size_t decimal = 0;

    for (size_t i = 0; i < str.length();i++)
        decimal = decimal * 10 + hexa.find(str[i]);
    return decimal;
}

Request::Request() : _request(), _conf(), _location(), _path(), _body(), _queries(), _extention(), _chunkedBodySize(),
                _chunkState(UNDONE), _fdBody(), _bodySize(0), _chunkSize(0), _status(200), _contentLength(0),
                _badFormat(0)
{
    this->_fdBody.close();
}

Request& Request::operator=(const Request& other)
{
    this->_request = other._request;
    this->_conf = other._conf;
    this->_location = other._location;
    this->_path = other._path;
    this->_body = other._body;
    this->_queries = other._queries;
    this->_extention = other._extention;
    this->_chunkedBodySize = other._chunkedBodySize;
    this->_chunkState = other._chunkState;
    this->_bodySize = other._bodySize;
    this->_chunkSize = other._chunkSize;
    this->_status = other._status;
    this->_contentLength = other._contentLength;
    this->_badFormat = other._badFormat;

    return *this;
}
Request::~Request()
{
}

bool Request::is_directory(const char *path)
{
	struct stat buf;
	stat(path, &buf);
	return S_ISDIR(buf.st_mode);
}

bool Request::is_file(const char *path)
{
    struct stat buf;
    stat(path, &buf);
    return S_ISREG(buf.st_mode);
}

void Request::setBodyPath()
{
    std::string upload = this->_location.getUpload();
    std::string dirPath;
    if (!upload.empty())
    {
        std::string path = "./" + this->_location.getRoot() + "/" + upload;
        if (is_file(path.c_str()))
        {
            this->_status = 400;
            this->_chunkState = DONE;
            return ;
        }
        else if (is_directory(path.c_str()))
        {
            if (access(path.c_str(), W_OK) == 0)
                dirPath = path + "/";
            else
            {
                this->_status = 403;
                this->_chunkState = DONE;
                return ;
            }
        }
        else
        {
            this->_status = 404;
            this->_chunkState = DONE;
            return ;
        }
    }
    else
        dirPath = "/tmp/";
    this->_body = dirPath + generateRandomFile() + this->_extention;
    this->_fdBody.open(this->_body, std::ios::app);
}

void Request::setFullBody(char *request, int &r)
{
    if (!this->_bodySize)
    {
        this->_bodySize = stringToDecimal(this->_request["Content-Length"]);
        this->_contentLength = this->_bodySize;
    }
    if (this->_body.empty())
    {
        this->setBodyPath();
        if (this->_status != 200)
            return ;
    }
    for (int i = 0; i < r; i++)
        this->_fdBody << request[i];
    this->_bodySize -= r;
    if (this->_bodySize)
        return ;
    this->_fdBody.close();
    _chunkState = DONE;
}

int Request::readChunkedBody(char *request, int &r)
{
    for (int i = 0; i < r; i++)
    {
        this->_fdBody << request[i];
        this->_chunkSize++;
        if (this->_chunkSize == this->_bodySize)
            return i + 1;
    }
    return r;
}

void Request::setChunkedBody(char *request, int &r)
{
    int chunkRead = 0;

    again:
    if (!this->_bodySize)
    {
        int i = 0;
        if (request[0] == '\r' && request[1] == '\n' && isHexa(request[2]))
            i = 2;
        else if (request[0] == '\n' && isHexa(request[1]))
            i = 1;
        while (isHexa(request[i]))
        {
            this->_chunkedBodySize.push_back(request[i]);
            i++;
        }
        if (request[i] == '\0')
            return ;
        if (request[i] != '\r' && request[i + 1] != '\n')
        {
            this->_status = 400;
            this->_chunkState = DONE;
            return ;
        }
        this->_bodySize = hexaToDecimal(this->_chunkedBodySize);
        request += i + 2;
        r -= i + 2;
        this->_chunkedBodySize.clear();
        if (this->_bodySize == 0)
        {
            std::cout << "done" << std::endl;
            this->_fdBody.close();
            this->_chunkState = DONE;
            return ;
        }
    }
    if (this->_body.empty())
    {
        this->setBodyPath();
        if (this->_status != 200)
            return ;
    }
    chunkRead = this->readChunkedBody(request, r);
    if (this->_chunkSize == this->_bodySize)
    {
        this->_contentLength += this->_bodySize;
        chunkRead += 2;
        this->_chunkSize = 0;
        this->_bodySize = 0;
        r -= chunkRead;
        request += chunkRead;
        if (r <= 0)
            return ;
        goto again;
    }
}

void Request::setBody(char *request, int &r)
{
    std::string line;

    if (!this->_badFormat)
        this->badFormat();
    if (this->_status != 200)
    {
        this->_chunkState = DONE;
        return;
    }
    if (this->_request.find("Transfer-Encoding") != this->_request.end())
        setChunkedBody(request, r);
    else
        setFullBody(request, r);
}

void Request::checkMethod()
{
    std::vector<std::string> limitExcept = this->_location.getLimit_except();
    std::string url = this->_request["URL"];
    std::string pattern = this->_location.getPattren();
    std::string method = this->_request["Method"];
    std::vector<std::string>::iterator it = limitExcept.begin();

    for(; it != limitExcept.end(); it++)
    {
        if ((*it) == method)
        {
            if(url.length() != pattern.length())
            {
                if (pattern == "/")
                    this->_path = this->_location.getRoot() + url;
                else
                    this->_path = this->_location.getRoot() + url.substr(pattern.length());
            }
            else
                this->_path = this->_location.getRoot();
            break;
        }
    }
    
    if (it == limitExcept.end() || (method != "POST" && method != "DELETE" && method != "GET"))
        this->_status = 405;
}

void Request::checkLocation()
{
    std::vector<Location> locations = this->_conf.getLocations();
    std::vector<Location>::iterator it = locations.begin();
    std::string upper;
    std::string url = this->_request["URL"];

    for(;it != locations.end();it++)
    {
        std::string pattern = (*it).getPattren();
        if (url.length() <  pattern.length())
            continue ;
        std::string lower = url.substr(0, pattern.length());
        if (pattern == "/" || (pattern == lower && (url[pattern.length()] == '\0' || url[pattern.length()] == '/')))
        {
            if (upper.empty())
            {
                upper = lower;
                this->_location = *it;
            }
            else
            {
                if (lower.length() > upper.length())
                {
                    upper = lower;
                    this->_location = *it;
                }
            }
        }
    }
    if (upper.empty())
        this->_status = 404;
}

void Request::setContentTypePost(std::string const & content)
{
    if (!contentStatePost)
    {
        for (int i = 0; i < 76; i++)
            cntPost[contentType[i]] = mimeType[i];
        contentStatePost = 1;
    }
    RequestMap::iterator it = cntPost.find(content);
    if (it != cntPost.end())
        this->_extention = it->second;
    else
        this->_extention = ".txt";
}


std::string    Request::get_attribute(const std::string& key)
{
    return _request[key];
}

void Request::setContentTypeGet(std::string const & content)
{
    if (!contentStateGet)
    {
        for (int i = 0; i < 76; i++)
            cntGet[mimeType[i]] = contentType[i];
        contentStateGet = 1;
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
    this->_request["Content-Type"] = "text/plain";
    return ;
    dotfound:
    std::string extention = content.substr(dot);
    RequestMap::iterator it = cntGet.find(extention);
    this->_request["Content-Type"] =  it->second;
}

std::string Request::decodeUrl(std::string const &url)
{
    std::string decoded;
    std::string hexa = "0123456789abcdef";
    size_t pos = url.find('%');

    if (pos == std::string::npos)
        return url;
    decoded = url.substr(0, pos);
    if (pos + 2 >= url.length())
    {
        this->_status = 400;
        this->_chunkState = DONE;
    }
    else if (!isHexa(url[pos + 1]) && !isHexa(url[pos + 2]))
    {
        this->_status = 400;
        this->_chunkState = DONE;
    }
    else
    {
        decoded += (int)hexaToDecimal(url.substr(pos + 1, 2));
        decoded += decodeUrl(url.substr(pos + 3));
    }
    return decoded;
}

std::string Request::decipherUrl(std::string const &url)
{
    std::string decoded = url;

    while (decoded.find('%') != std::string::npos)
        decoded = this->decodeUrl(decoded);
    if (decoded.find('?') != std::string::npos)
    {
        this->_queries = decoded.substr(decoded.find('?') + 1);
        decoded = decoded.substr(0, decoded.find('?'));
    }
    return decoded;
}

void Request::parseUrl(std::string const &line)
{
    std::string split;
    std::istringstream header(line);

    if (line == "\0")
        goto badline;
    std::getline(header, split, ' ');
    if (split == "\0" || (split != "POST" && split != "GET" && split != "DELETE"))
        goto badline;
    this->_request["Method"] = split;
    std::getline(header, split, ' ');
    if (split == "\0" || split[0] != '/')
        goto badline;
    this->_request["URL"] = this->decipherUrl(split);
    std::getline(header, split, '\r');
    if (split == "\0" || split != "HTTP/1.1")
        goto badline;
    this->_request["Protocol"] = split;
    return;
    badline:
    this->_status = 400;
    this->_chunkState = DONE;
}

void Request::badFormat()
{
    RequestMap::iterator transferEncoding = this->_request.find("Transfer-Encoding");
    RequestMap::iterator contentLength = this->_request.find("Content-Length");
    std::string url = this->_request.find("URL")->second;

    this->checkLocation();
    if (transferEncoding != this->_request.end() && transferEncoding->second != "chunked" && this->_request["Method"] == "POST")
        this->_status = 501;
    else if (transferEncoding != this->_request.end() && contentLength != this->_request.end())
        this->_status = 400;
    else if (transferEncoding == this->_request.end() && contentLength == this->_request.end() && this->_request["Method"] == "POST")
        this->_status = 400;
    else if (!(std::find_if(url.begin(), url.end(), invalidUrl()) != url.end()))
        _status = 400;
    else if (url.length() > 2048)
        this->_status = 414;
    else if (contentLength != this->_request.end() && stringToDecimal(contentLength->second) > this->_location.getClientMaxBodySize())
        this->_status = 413;
    if (this->_status == 200)
        this->checkMethod();
    this->_badFormat = 1;
}

void Request::checkServerName(Webserver::ServerMap const & servers)
{
    std::string serverName = this->_request["Host"];

    for (Webserver::ServerMap::const_iterator it = servers.begin(); it != servers.end(); it++)
    {
        if (it->second->() == serverName)
        {
            this->_conf = *it->second;
            return ;
        }
    }
}

void Request::parseRequest(char *request, Webserver::ServerMap const & servers, int &r)
{
    std::string line;
    std::istringstream req(request);
    int bodySize = r;

    if (!this->_request.empty())
        goto setbody;
    std::getline(req, line);
    bodySize -= line.length() + 1;
    this->parseUrl(line);
    if (this->_status != 200)
        return ;
    while (std::getline(req, line))
    {
        bodySize -= line.length() + 1;
        if (line == "\r")
            goto setbody;
        size_t separator = line.find(": ");
        if (separator != std::string::npos)
            this->_request[line.substr(0, separator)] = line.substr(separator + 2, line.length() - separator - 3);
    }
    checkServerName(servers);
    setbody:
    std::string method = this->_request["Method"];
    if (method == "POST")
    {
        this->setContentTypePost(this->_request["Content-Type"]);
        this->setBody(request + (r - bodySize), bodySize);
    }
    else
    {
        this->setContentTypeGet(this->_request["URL"]);
        this->badFormat();
        _chunkState = DONE;
    }
}


void Request::printElement()
{
    int i = 0;
    for(std::map<std::string, std::string>::iterator it = this->_request.begin();
        it != this->_request.end(); it++, i++)
        std::cout << i << " " << it->first << ": " << it->second << std::endl;
}

RequestMap const & Request::getRequest() const
{
    return this->_request;
}

int const &   Request::getStatus() const
{
    return this->_status;
}

std::string const &   Request::getPath() const
{
    return this->_path;
}

Location const &  Request::getLocation() const
{
    return this->_location;
}

ChunkState const &   Request::getChunkedState() const
{
    return this->_chunkState;
}

std::string const &   Request::getBody() const
{
    return this->_body;
}

size_t const &   Request::getContentLength() const
{
    return this->_contentLength;
}

std::string const &   Request::getQueries() const
{
    return this->_queries;
}