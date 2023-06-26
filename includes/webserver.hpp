
#pragma once

#include "configuration.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "client.hpp"





class Webserver
{
	public:
		typedef std::pair<fd_set, fd_set> SetsPair;
	private:
		ConfVec 	_configs;
		std::vector<Registry> _registry;
		std::vector<Client *> _clients;
		static fd_set _writeset;
		static fd_set _readset;
		static SOCKET _max_socket;
	public:
		Webserver();
		Webserver(std::string content);
		Webserver(const Webserver&  other);
		Webserver& operator=(const Webserver&  _servers);
		~Webserver();
		const std::vector<Registry>& get_registry() const;
		struct ServerException : public CustomeExceptionMsg
		{
    		ServerException(const std::string& message);
		};

		class WebserverReset : public CustomeExceptionMsg
		{
			public:
				WebserverReset();
    			WebserverReset(const std::string& message);
				virtual ~WebserverReset() throw();
		};
		ConfVec init_configs(std::string content);
		void init_registry();
		bool wait_on_client(SetsPair& sets);
		void drop_client(size_t i);
		void run();
		void stop();
		void reset();

		int fetch_request (Client *client);
		int send_response (Client *client);

		static void add_socket(SOCKET socket);
		static void clear_set();
};