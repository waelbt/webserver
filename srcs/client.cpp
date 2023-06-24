#include "../includes/webserver.hpp"


Client::Client() : _registry(), _address(), _address_length(sizeof(_address)), _request(), _response(), _data_sent(), _bytesSent(), _remaining()
{
    
}

Client::Client(const Registry& registry) : _registry(registry), _address(), _address_length(sizeof _address), _request(), _response(), _data_sent(), _bytesSent(), _remaining()
{
    std::cout << "procces " << getpid() << "paremt process   "<< getppid() << std::endl;
    std::cout << "_listen_socket "<< registry._listen_socket << std::endl;
    _socket = accept(registry._listen_socket, NULL, NULL);
    (_socket <= 0) ?  throw ClientException("establish a connection with a client failed") : (NULL);
    (fcntl(_socket,F_SETFL,O_NONBLOCK) == -1) ? throw ClientException("failed to set socket descriptor to non-blocking mod") : (NULL);
    Webserver::add_socket(_socket);
    std::cout << "New connection from , socket " << get_client_address() << _socket << std::endl;
}

Client::ClientException::ClientException(const std::string& message) : CustomeExceptionMsg(message)
{
}

Client::Client(const Client& other)
{
    *this = other;
}

Client& Client::operator=(const Client& other)
{
    this->_registry = other._registry;
    this->_address = other._address;
    this->_address_length = other._address_length;
    this->_socket = other._socket;
	this->_response = other._response;
    this->_remaining = other._remaining;
    return *this;
}

std::string Client::get_client_address() 
{
    static char address_buffer[100];

    getnameinfo((struct sockaddr*)&_address, _address_length, address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
    return address_buffer;
}

Client::~Client()
{
    close(_socket);
}