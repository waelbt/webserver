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

fd_set Webserver::wait_on_client()
{
	fd_set reads =  _server_set;
	
    for (ServerMap::iterator it = _servers.begin(); it != _servers.end(); it++)
    {
        std::vector<Client> clients = it->second->get_clients();
	    for (size_t size = 0; size < clients.size(); size++)
	    {
	    	FD_SET(clients[size]._socket, &reads);
	    	if (clients[size]._socket > _max_socket)
               _max_socket = clients[size]._socket;
	    }
    }
	// i should set the timeout parameter 
	if (select(_max_socket + 1, &reads, 0, 0, 0) < 0)
		throw CustomeExceptionMsg("select() failed. ("+  std::string(strerror(errno)) + ")");
	return reads;
}

void Webserver::run()
{
    fd_set reads;
    // fd_set write;

    while (1)
    {
        reads = wait_on_client();
        for (ServerMap::iterator it = _servers.begin(); it != _servers.end(); it++)
        {
            std::vector<Client>& _client = it->second->get_clients();
    	    if (FD_ISSET(it->first, &reads))
		    	_client.insert(_client.end(), Client(it->first));
		    for (size_t i = 0; i < _client.size(); i++)
		    {
		    	if (FD_ISSET(_client[i]._socket, &reads))
		    	{
		    		int r = recv(_client[i]._socket, _client[i]._request + _client[i]._received, MAX_REQUEST_SIZE - _client[i]._received, 0);
		    		if (r < 1)
		    		{
		    			std::cout << "Unexpected disconnect from " << _client[i].get_client_address() << std::endl;
		    			it->second->drop_client(i);
		    		}
		    		else
		    		{
		    			_client[i]._received += r;
		    			_client[i]._request[_client[i]._received] = '\0';
						_client[i]._req.parseRequest(_client[i]._request);
		    			// std::cout << _client[i]._request << std::endl;
						_client[i]._req.printElement();
		    			// request handling
		    		}
		    		// add the client set to the writing set
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
