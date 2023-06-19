#include "../includes/server.hpp"

fd_set Webserver::_socketset;
fd_set Webserver::_writeset;
SOCKET Webserver::_max_socket = 0;

Webserver::Webserver() : _servers()
{

}

Webserver::Webserver(std::string content) : _servers()
{
	setup(content);
}

void Webserver::add_socket(SOCKET socket)
{
	FD_SET(socket, &_socketset);
	if (_max_socket < socket)
		_max_socket = socket;
}

void  Webserver::setup(std::string content)
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

	timeout.tv_sec = 0;
	timeout.tv_usec = 5;

	if (select(_max_socket + 1, &reads, 0, 0, &timeout) < 0)
		throw CustomeExceptionMsg("select() failed. ("+  std::string(strerror(errno)) + ")");
	return reads;
}

void  Webserver::clear_set()
{
	for (SOCKET fd = 0; fd <= Webserver::_max_socket; fd++) {
		if (FD_ISSET(fd, &Webserver::_socketset) || FD_ISSET(fd, &Webserver::_writeset))
			close(fd);
	}
	FD_ZERO(&_socketset);
	FD_ZERO(&_writeset);
}

int Webserver::fetch_request (Client *client, const Configuration& conf)
{
	char request[MAX_REQUEST_SIZE + 1] = {0};
	int r;

	r = recv(client->_socket, request, MAX_REQUEST_SIZE, MSG_DONTWAIT);
	if (r < 1)
	{
		std::cout << "Unexpected disconnect from " << client->get_client_address() << std::endl;
		FD_CLR(client->_socket, &_socketset);
		return 0;
	}
	else
	{
		request[r] = '\0';
		client->_request.parseRequest(request, conf, r);
		if (client->_request.getChunkedState() == DONE)
		{
			FD_CLR(client->_socket, &_socketset);
			FD_SET(client->_socket, &_writeset);
		}
	}
	return 1;
}

void Webserver::run()
{
    fd_set tmpset;

	while (1)
    {
        tmpset = wait_on_client();
        for (ServerMap::iterator it = _servers.begin(); it != _servers.end(); it++)
        {
            std::vector<Client *>& _client = it->second->get_clients();
    	    if (FD_ISSET(it->first, &tmpset))
		    	_client.insert(_client.end(), new Client(it->first));
		    for (size_t i = 0; i < _client.size(); i++)
		    {
		    	if (FD_ISSET(_client[i]->_socket, &tmpset))
		    	{
					if (!fetch_request(_client[i], it->second->get_configuration())) {
						it->second->drop_client(i); continue ; }
				}
				// if (FD_ISSET(_client[i]->_socket, &_writeset))
				// {
				// 	if(send_response(_client[i]))
				// 		it->second->drop_client(i);
				// }
		    }
        }
    }
	this->stop();
}

int Webserver::send_response(Client *client)
{
	if (!client->_data_sent.length())
	{
		client->_response.serveResponse(client->_request);
		if (client->_response.getIsHeaderParsed() && !client->_response.getIsHeaderSent())
			client->_data_sent = client->_response.sendHeader();
		else
			client->_data_sent = client->_response.getBody();
	}
	client->_bytesSent = send(client->_socket, client->_data_sent.c_str(), client->_data_sent.size(), 0);
	if (client->_response.getIsBodySent() == true || (client->_bytesSent == -1)) {
		FD_CLR(client->_socket, &_writeset); return 1; }
	if ((size_t)client->_bytesSent < client->_data_sent.length()) {
		client->_data_sent = client->_data_sent.substr(client->_bytesSent, client->_data_sent.length()); return 0;}
	client->_data_sent.clear();
	return 0;
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
