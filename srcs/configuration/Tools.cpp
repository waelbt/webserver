#include "../../includes/Configuration.hpp"


void PortValidator::operator()(int port) {
    if (port < 0 || port > 65535)
        throw CustomeExceptionMsg(InvalidPort);
}

std::string TokenToString::operator()(TokenPair& pair)
{
    return pair.first;
}

CustomeExceptionMsg::CustomeExceptionMsg() : _message() {
}

CustomeExceptionMsg::CustomeExceptionMsg(const std::string& message) : _message(message) {
}

const char* CustomeExceptionMsg::what() const throw()
{
    return _message.c_str();
}

CustomeExceptionMsg::~CustomeExceptionMsg() throw()
{}

bool is_space(const char& c) {
    return !(std::isspace(static_cast<unsigned char>(c)));
}

bool is_symbol(const char& c) {
    static std::string EntityType("{;}");
    return (EntityType.find(std::string(1, c)) != std::string::npos);
}

bool is_directive(const char& c) {
    static std::string EntityType(":;");
    return (EntityType.find(std::string(1, c)) != std::string::npos);
}

bool is_semicolon(const char& c) {
    return ((c == ',') || (c == '\0'));
}

void string_trim(TokenPair& pair) {
    StrIter begin = std::find_if(pair.first.begin(), pair.first.end(), is_space);
    if (begin != pair.first.begin())
        pair.first.erase(pair.first.begin(), begin);
    StrIter end = std::find_if(pair.first.rbegin(), pair.first.rend(), is_space).base();
    if (end != pair.first.end())
        pair.first.erase(end, pair.first.end());
}

TokenPair selectToken(StrIter& begin, const StrIter& end, int& level, bool (*func)(const char&))
{
	std::string value;
    StrIter iter;

	iter = std::find_if(begin, end, func);
    value = std::string(begin, iter);
    begin = iter + 1;
    level += (*iter == BLOCK) * 1 +  (*iter == END_BLOCK) * -1;
	return std::make_pair(value, *iter);
}

void is_integer(std::string num)
{
    if(std::find_if(num.begin(), num.end(), isalpha) != num.end())
        throw CustomeExceptionMsg("failed to convert " + num + " to an integer");
}

long long to_integer(const std::string& string)
{
    long long num;
    std::stringstream strstrean(string);
    
    is_integer(string);
    strstrean >> num;
    return num;
}

std::vector<TokenPair> SplitValues(std::string value, bool (*func)(const char&))
{
    int level = 0;
    std::vector<TokenPair> res;
    std::pair<StrIter, StrIter> it(std::make_pair(value.begin(), value.end()));

    while (it.first < it.second)
        res.insert(res.end(), selectToken(it.first, it.second, level, func));
    (level) ? (throw CustomeExceptionMsg(SyntaxError)) : (NULL);
    return res;
}

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

s_err_pages::s_err_pages(std::string value)
{
    std::vector<std::string>  values(converter(value, TokenToString()));
    std::vector<std::string>::iterator end = values.end();

    std::transform(values.begin(), --end, std::back_inserter(this->_status), to_integer);
    std::for_each(_status.begin(), _status.end(), PortValidator());
    _page = *end;
}