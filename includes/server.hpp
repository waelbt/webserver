
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
	std::string _data_sent;
	ssize_t _bytesSent;
	bool _remaining; 


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
		std::vector<Client *> _client;
    public:
		Server();
		Server(const Configuration& conf);
		Server(const Server& other);
		Server& operator=(const Server& other);

		SOCKET server_socket(std::string host, std::string port);
		class ServerException : public CustomeExceptionMsg
		{
			public:
				ServerException();
    			ServerException(const std::string& message);
				virtual ~ServerException() throw();
		};
		std::vector<Client *>& get_clients();
		SOCKET get_listen_sockets() const;
		Configuration get_configuration() const;

		static void setnonblocking(int sock);
		void drop_client(size_t i);
		void showConfig() const;

		void clear_clients();
		~Server();
};


class Webserver
{
	public:
		typedef std::pair<fd_set, fd_set> SetsPair;
		typedef std::map<SOCKET, Server*> ServerMap;
	private:
		ServerMap  _servers;
		static fd_set _writeset;
		static fd_set _readset;
		static SOCKET _max_socket;
	public:
		Webserver();
		Webserver(std::string content);
		Webserver(const Webserver&  other);
		Webserver& operator=(const Webserver&  _servers);
		~Webserver();

		void setup(std::string content);
		bool wait_on_client(SetsPair& sets);
		void run();
		void stop();
		void reset();

		int fetch_request (Client *client);
		int send_response (Client *client);

		static void add_socket(SOCKET socket);
		static void clear_set();		
};