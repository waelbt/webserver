
#pragma once

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
#include "client.hpp"


// s_addrinfo *get_s_addrinfo(std::string host, std::string port);

// struct server_infos
// {
// 	SOCKET		_listen_sockets;
// 	std::string	_servername;
// 	std::string _port;
// 	std::string _host;
// 	int			_conf_id;
// };

// class Server
// {
// 	public:
// 		typedef std::vector<Client>::iterator clientIter;
//     private:
//         Configuration _conf;
// 		SOCKET _listen_sockets;
// 		std::vector<Client *> _client;
//     public:
// 		Server();
// 		Server(const Configuration& conf);
// 		Server(const Server& other);
// 		Server& operator=(const Server& other);

// 		// SOCKET server_socket(std::string host, std::string port);
// 		class ServerException : public CustomeExceptionMsg
// 		{
// 			public:
// 				ServerException();
//     			ServerException(const std::string& message);
// 				virtual ~ServerException() throw();
// 		};
// 		std::vector<Client *>& get_clients();
// 		SOCKET get_listen_sockets() const;
// 		Configuration get_configuration() const;

// 		static void setnonblocking(int sock);
// 		void drop_client(size_t i);
// 		void showConfig() const;

// 		void clear_clients();
// 		~Server();
// };


class Webserver
{
	public:
		typedef std::pair<fd_set, fd_set> SetsPair;
		// typedef std::vector<Server*> ServerVec;
	public:
		ConfVec 	_configs;
		std::vector<Registry> _registry;
		std::vector<Client *> _clients;
		// ServerVec  _servers;
		static fd_set _writeset;
		static fd_set _readset;
		static SOCKET _max_socket;
	public:
		Webserver();
		Webserver(std::string content);
		Webserver(const Webserver&  other);
		Webserver& operator=(const Webserver&  _servers);
		~Webserver();
	
		class WebserverReset : public CustomeExceptionMsg
		{
			public:
				WebserverReset();
    			WebserverReset(const std::string& message);
				virtual ~WebserverReset() throw();
		};
		ConfVec init_configs(std::string content);
		void get_registry();
		bool wait_on_client(SetsPair& sets);
		void drop_client(size_t i);
		void run();
		void stop();
		// void reset();

		int fetch_request (Client *client);
		int send_response (Client *client);

		static void add_socket(SOCKET socket);
		static void clear_set();		
};