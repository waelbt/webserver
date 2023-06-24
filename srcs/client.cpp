#include "../includes/server.hpp"


Client::Client() : _registry(), _address(), _address_length(sizeof(_address)), _request(), _response(), _data_sent(), _bytesSent(), _remaining()
{
    
}

Client::Client(const Registry& registry) : _registry(registry), _address(), _address_length(sizeof _address), _request(), _response(), _data_sent(), _bytesSent(), _remaining()
{
    std::cout << "procces " << getpid() << "paremt process   "<< getppid() << std::endl;
    std::cout << "_listen_socket "<< registry._listen_socket << std::endl;
    _socket = accept(registry._listen_socket, NULL, NULL);
    if (_socket <= 0)
        throw Webserver::WebserverReset("accept() fail");
    fcntl(_socket,F_SETFL,O_NONBLOCK);
    // if (fcntl(_socket, F_SETFL, fcntl(_socket, F_GETFL, 0) | O_NONBLOCK) == -1) {
	// 	close(_socket);
    //     throw "client setsocket file "; // catch it later
	// }
    Webserver::add_socket(_socket);
    std::cout << "New connection from , socket " << get_client_address() << _socket << std::endl;
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