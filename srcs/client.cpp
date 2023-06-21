#include "../includes/server.hpp"


Client::Client() : _server_socket(), _address(), _address_length(sizeof(_address)), _request(), _response(), _data_sent(), _bytesSent(), _remaining()
{
    
}

Client::Client(SOCKET server_socket) : _server_socket(server_socket), _address(), _address_length(sizeof _address), _request(), _response(), _data_sent(), _bytesSent(), _remaining()
{
    std::cout << "procces " << getpid() << "paremt process   "<< getppid() << std::endl;
    std::cout << "_server_socket "<< _server_socket << std::endl;
    _socket = accept(_server_socket, NULL, NULL);
    if (_socket <= 0)
        throw CustomeExceptionMsg("accept() failed. ("+  std::string(strerror(errno)) + ")");
    Server::setnonblocking(_socket);
    Webserver::add_socket(_socket);
    std::cout << "New connection from , socket " << get_client_address() << _socket << std::endl;
}


Client::Client(const Client& other)
{
    *this = other;
}

Client& Client::operator=(const Client& other)
{
    this->_server_socket = other._server_socket;
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