#include "../includes/server.hpp"

Client::Client(SOCKET server_socket) : _server_socket(server_socket), _address(), _address_length(sizeof(_address))
{
    _socket = accept(_server_socket, (struct sockaddr*) &(_address), &(_address_length));
    if (_socket < 0)
        throw CustomeExceptionMsg("socket() failed. ("+  std::string(strerror(errno)) + ")");
    std::cout << "New connection from " << get_client_address() << std::endl;
}

SOCKET Client::ClientSocket() const
{
    return _socket;
}

SOCKET Client::ServerSocket() const
{
    return _server_socket;
}

socklen_t Client::GetAddressLen() const
{
    return _address_length;
}

std::string Client::get_client_address() 
{
    static char address_buffer[100];

    getnameinfo((struct sockaddr*)&_address, _address_length, address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
    return address_buffer;
}