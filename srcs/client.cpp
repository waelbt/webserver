#include "../includes/server.hpp"

Client::Client(SOCKET server_socket) : _server_socket(server_socket), _address(), _address_length(sizeof(_address)), _received(0)
{
    memset(_request, 0, sizeof(_request));
    _socket = accept(_server_socket, (struct sockaddr*) &(_address), &(_address_length));
    if (_socket < 0)
        throw CustomeExceptionMsg("socket() failed. ("+  std::string(strerror(errno)) + ")");
    std::cout << "New connection from " << get_client_address() << std::endl;
}

std::string Client::get_client_address() 
{
    static char address_buffer[100];

    getnameinfo((struct sockaddr*)&_address, _address_length, address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
    return address_buffer;
}