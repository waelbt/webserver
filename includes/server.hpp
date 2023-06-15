
#include "configuration.hpp"


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "request.hpp"
#include "response.hpp"


typedef int	SOCKET;
typedef struct addrinfo s_addrinfo;
typedef struct sockaddr_storage s_sockaddr_storage;

struct Client {
	// attributes
	SOCKET _server_socket;
	s_sockaddr_storage _address;
	socklen_t _address_length;
	SOCKET _socket;
	Request _request;
	Response _response;

	// methods
	Client();
	Client(SOCKET server_socket);
	Client(const Client& other);
	Client& operator=(const Client& other);
	std::string get_client_address();
	~Client();
};


// s_addrinfo *get_s_addrinfo(std::string host, std::string port);

class Server
{
	public:
		typedef std::vector<Client>::iterator clientIter;
    private:
        Configuration _conf;
		SOCKET _listen_sockets;
		std::vector<Client> _client;
    public:
		Server();
		Server(const Configuration& conf);
		Server(const Server& other);
		Server& operator=(const Server& other);

		void setup_server_socket(std::string host, std::string port);
		class ServerException : public CustomeExceptionMsg
		{
			public:
				ServerException();
    			ServerException(const std::string& message);
				virtual ~ServerException() throw();
		};
		std::vector<Client>& get_clients();
		SOCKET get_listen_sockets() const;
		Configuration get_configuration() const;

		static void setnonblocking(int sock);
		void drop_client(size_t i);
		void showConfig() const;

		~Server();
};


class Webserver
{
	public:
		typedef std::map<SOCKET, Server*> ServerMap;
		static SOCKET _max_socket;
		static fd_set _socketset;
		//static fd_set writes;
	private:
		ServerMap  _servers;
	public:
		Webserver();
		Webserver(std::string content);
		Webserver(const Webserver&  other);
		Webserver& operator=(const Webserver&  _servers);
		void fill_servers(std::string content);
		static void add_socket(SOCKET socket);
		static void clear_set();
		fd_set wait_on_client();
		void run();
		void stop();
		~Webserver();
};