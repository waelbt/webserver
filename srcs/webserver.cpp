#include "../includes/server.hpp"

fd_set Webserver::_socketset;
fd_set Webserver::writes;
SOCKET Webserver::_max_socket = 0;
Webserver::Webserver() : _servers()
{

}

Webserver::Webserver(std::string content) : _servers()
{
	fill_servers(content);
}

void Webserver::add_socket(SOCKET socket)
{
	FD_SET(socket, &_socketset);
	if (_max_socket < socket)
		_max_socket = socket;
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
					std::cout << "SERVER ("+  to_string(_servers.size() + 1) + ") EXCEPTION : " << e.what() << std::endl;
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
	_socketset = other._socketset;
	_max_socket = other._max_socket;
	return *this;
}


fd_set Webserver::wait_on_client()
{
	struct timeval timeout;
	fd_set reads(_socketset);

	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	if (select(_max_socket + 1, &reads, 0, 0, &timeout) < 0)
		throw CustomeExceptionMsg("select() failed. ("+  std::string(strerror(errno)) + ")");
	return reads;
}

void  Webserver::clear_set()
{
	FD_ZERO(&_socketset);
}

void Webserver::run()
{
    fd_set tmpset;

	FD_ZERO(&writes);
	while (1)
    {
        tmpset = wait_on_client();
        for (ServerMap::iterator it = _servers.begin(); it != _servers.end(); it++)
        {
            std::vector<Client>& _client = it->second->get_clients();
    	    if (FD_ISSET(it->first, &tmpset))
		    	_client.insert(_client.end(), Client(it->first));
		    for (size_t i = 0; i < _client.size(); i++)
		    {
		    	if (FD_ISSET(_client[i]._socket, &tmpset))
		    	{
					char request[MAX_REQUEST_SIZE + 1] = {0};
		    		int r = recv(_client[i]._socket, request, MAX_REQUEST_SIZE, MSG_DONTWAIT);
		    		if (r < 1)
		    		{
		    			std::cout << "Unexpected disconnect from " << _client[i].get_client_address() << std::endl;
						FD_CLR(_client[i]._socket, &_socketset);
		    			it->second->drop_client(i);
						continue;
		    		}
		    		else
		    		{
		    			request[r] = '\0';
						_client[i]._request.parseRequest(request, it->second->get_configuration());
						if (true)
						{
							FD_CLR(_client[i]._socket, &_socketset);
							FD_SET(_client[i]._socket, &writes);
						}
					}
		    	}
				if (FD_ISSET(_client[i]._socket, &writes))
				{
					_client[i]._response.get(_client[i]._request);
					send(_client[i]._socket, _client[i]._response.toString().c_str(), _client[i]._response.toString().length(), 0);
					FD_CLR(_client[i]._socket, &writes);
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
		close(it->first);
}

Webserver::~Webserver()
{
    for (ServerMap::iterator it = _servers.begin(); it != _servers.end(); it++)
    {
        delete it->second;
    }
}
