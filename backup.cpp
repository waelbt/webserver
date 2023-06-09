#include "../includes/server.hpp"

Webserver::Webserver()
{

}


Webserver::Webserver(const ServerMap&  servers) : _servers(servers), _max_socket()
{
	FD_ZERO(&_server_set);
	for (ServerMap::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		FD_SET(it->first, &_server_set);
		if (_max_socket < it->first)
			_max_socket = it->first;
	}
}


Webserver::Webserver(const Webserver&  other)
{
	*this = other;
}

Webserver& Webserver::operator=(const Webserver&  other)
{
	_servers = other._servers;
	_server_set = other._server_set;
	_max_socket = other._max_socket;
	return *this;
}

std::pair<fd_set, fd_set> Webserver::wait_on_client()
{
	fd_set reads =  _server_set;
	fd_set writes = _server_set;
	
    for (ServerMap::iterator it = _servers.begin(); it != _servers.end(); it++)
    {
        std::vector<Client> clients = it->second->get_clients();
	    for (size_t size = 0; size < clients.size(); size++)
	    {
	    	FD_SET(clients[size]._socket, &reads);
			FD_SET(clients[size]._socket, &writes);
	    	if (clients[size]._socket > _max_socket)
               _max_socket = clients[size]._socket;
	    }
    }
	// i should set the timeout parameter 
	if (select(_max_socket + 1, &reads, &writes, 0, 0) < 0)
		throw CustomeExceptionMsg("select() failed. ("+  std::string(strerror(errno)) + ")");
	return std::make_pair(reads, writes);
}

void Webserver::run()
{
    std::pair<fd_set, fd_set> sets;


    while (1)
    {
        sets = wait_on_client();
        for (ServerMap::iterator it = _servers.begin(); it != _servers.end(); it++)
        {
            std::vector<Client>& _client = it->second->get_clients();
    	    if (FD_ISSET(it->first, &sets.first))
		    	_client.insert(_client.end(), Client(it->first));
		    for (size_t i = 0; i < _client.size(); i++)
		    {
		    	if (FD_ISSET(_client[i]._socket, &sets.first))
		    	{
					char request[MAX_REQUEST_SIZE + 1] = {0};
		    		int r = recv(_client[i]._socket, request, MAX_REQUEST_SIZE, MSG_DONTWAIT);
		    		if (r < 1)
		    		{
		    			std::cout << "Unexpected disconnect from " << _client[i].get_client_address() << std::endl;
						FD_CLR(_client[i]._socket, &sets.first);
		    			it->second->drop_client(i);
		    		}
		    		else
		    		{
		    			request[r] = '\0';
						_client[i]._req.parseRequest(request);
						if (_client[i]._req.eof()) //response how li 5asso i flagi liya 
						{
							_client[i]._req.printElement();
							FD_CLR(_client[i]._socket, &sets.first);
						}
		    		}
		    	}
				if (FD_ISSET(_client[i]._socket, &sets.second))
				{
					const char *c400 = "HTTP/1.1 400 Bad Request\r\n"
    				"Connection: close\r\n"
    				"Content-Length: 11\r\n\r\nBad Request";
   					send(_client[i]._socket, c400, strlen(c400), 0);
					FD_CLR(_client[i]._socket, &sets.second);
   					it->second->drop_client(i);
				}
		    }
        }
    }
	this->stop();
}

void Webserver::stop()
{
	for (ServerMap::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		close(it->first);
	}
}

Webserver::~Webserver()
{
    for (ServerMap::iterator it = _servers.begin(); it != _servers.end(); it++)
    {
        delete it->second;
    }
}
