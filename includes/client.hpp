#include "configuration.hpp"
#include "request.hpp"
#include "response.hpp"
#include "Registry.hpp"

typedef int	SOCKET;
typedef struct addrinfo s_addrinfo;
typedef struct sockaddr_storage s_sockaddr_storage;


struct Client {
	// attributes
	Registry _registry;
	s_sockaddr_storage _address;
	socklen_t _address_length;
	SOCKET _socket;
	Request _request;
	Response _response;
	std::string _data_sent;
	ssize_t _bytesSent;
	bool _remaining;

	// nested class
	struct ClientException : public CustomeExceptionMsg
	{
    	ClientException(const std::string& message);
	};

	// methods
	Client();
	Client(const Registry& registry);
	Client(const Client& other);
	Client& operator=(const Client& other);
	std::string get_client_address();
	~Client();
};