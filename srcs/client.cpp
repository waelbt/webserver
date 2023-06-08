#include "../includes/server.hpp"


Client::Client() : _server_socket(), _address(), _address_length(sizeof(_address)), _req()
{
    
}

Client::Client(SOCKET server_socket) : _server_socket(server_socket), _address(), _address_length(sizeof(_address)), _req()
{
    // memset(_request, 0, sizeof(_request));
    _socket = accept(_server_socket, (struct sockaddr*) &(_address), &(_address_length));
    if (_socket < 0)
        throw CustomeExceptionMsg("socket() failed. ("+  std::string(strerror(errno)) + ")");
    std::cout << "New connection from " << get_client_address() << std::endl;
}


Client::Client(const Client& other)
{
    *this = other;
}

Client& Client::operator=(const Client& other)
{
    _server_socket = other._server_socket;
    _address = other._address;
    _address_length = other._address_length;
    _socket = other._socket;
    // _request = other._request;
    // _received = other._received;
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
    // close(_socket);
}