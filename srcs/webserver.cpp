#include "../includes/server.hpp"

Webserver::Webserver() : _servers(), _max_socket()
{

}

Webserver::Webserver(std::string content) : _servers(), _max_socket()
{
	fill_servers(content);
	FD_ZERO(&_server_set);
	for (ServerMap::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		FD_SET(it->first, &_server_set);
		if (_max_socket < it->first)
			_max_socket = it->first;
	}
}


void  Webserver::fill_servers(std::string content)
{
	TokenVects 									data(SplitValues(content));
	std::pair<TokenVectsIter, TokenVectsIter>	it(std::make_pair(data.begin(),  data.end()));

	while (it.first < it.second)
	{
		if (it.first->second == BLOCK)
		{
			string_trim(*(it.first));
			if (it.first->first == "server")
			{
				try
				{
					Server  *tmp = new Server(Configuration(it.first, it.second));
					_servers[tmp->get_listen_sockets()] = tmp;
				}
				catch(Server::ServerException& e)
				{
					std::cout << "SERVER ("+  std::to_string(_servers.size() + 1) + ") EXCEPTION : " << e.what() << std::endl;
					std::cout << "NOTE : the other servers will continue their work perfectly." << std::endl;
				}
			}
			else
				throw CustomeExceptionMsg(it.first->first + BlockErro);
		}
		else if ((it.first->second != END))
			throw CustomeExceptionMsg(it.first->first + BlockErro);
		it.first++;
	}
	if (_servers.empty())
		throw CustomeExceptionMsg(EmptyFile);
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
	struct timeval tv;
	std::pair<fd_set, fd_set> sets(_server_set, _server_set);
	
	tv.tv_sec = 0;
	tv.tv_usec = 0;
    for (ServerMap::iterator it = _servers.begin(); it != _servers.end(); it++)
    {
        std::vector<Client> clients = it->second->get_clients();
	    for (size_t size = 0; size < clients.size(); size++)
	    {
	    	FD_SET(clients[size]._socket, &sets.first);
			FD_SET(clients[size]._socket, &sets.second);
	    	if (clients[size]._socket > _max_socket)
               _max_socket = clients[size]._socket;
	    }
    }
	// i should set the timeout parameter 
	if (select(_max_socket + 1, &sets.first, &sets.second, 0, &tv) < 0)
		throw CustomeExceptionMsg("select() failed. ("+  std::string(strerror(errno)) + ")");
	return sets;
}

void Webserver::run()
{
    std::pair<fd_set, fd_set> sets;
    // fd_set write;

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
						_client[i]._request.parseRequest(request, it->second->get_configuration());
						// _client[i]._request.printElement();
		    		}
		    	}
				// if (FD_ISSET(_client[i]._socket, &writes))
				// {
				// 	// send
				// }
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
