#include "../includes/webserver.hpp"

fd_set Webserver::_readset;
fd_set Webserver::_writeset;
SOCKET Webserver::_max_socket = 0;

Webserver::Webserver()
{

}


bool compare(Configuration a, Configuration b)
{
	if((a.getHost() == b.getHost()) && a.getPort() == b.getPort())
		return true;
	return false;
}


Webserver::ServerException::ServerException(const std::string& message) : CustomeExceptionMsg(message)
{
}

SOCKET server_socket(std::string host, std::string port)
{
	int opt = 1;
	SOCKET listen_sockets;
	s_addrinfo hints;
    s_addrinfo *bind_addr;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(host.c_str(), port.c_str(), &hints, &bind_addr);
	if (!bind_addr) {
		freeaddrinfo(bind_addr);
		throw Webserver::ServerException("getaddrinfo system call failed.");
	}
	listen_sockets = socket(bind_addr->ai_family, bind_addr->ai_socktype, bind_addr->ai_protocol);
	(listen_sockets < 0) ? throw Webserver::ServerException("socket() system call failed") : (NULL);
	(fcntl(listen_sockets,F_SETFL,O_NONBLOCK) == -1) ? throw Webserver::ServerException("failed to set socket descriptor to non-blocking mod") : (NULL);
	if (setsockopt(listen_sockets, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
	{
		close(listen_sockets); freeaddrinfo(bind_addr);
		throw Webserver::ServerException("set sock option failed");
	}
	if(bind(listen_sockets, bind_addr->ai_addr, bind_addr->ai_addrlen))
	{
		close(listen_sockets); freeaddrinfo(bind_addr);
		throw Webserver::ServerException("bind system call failed");
	}
	if (listen(listen_sockets, SOMAXCONN) < 0)
	{
		close(listen_sockets); freeaddrinfo(bind_addr);
		throw Webserver::ServerException("listen system call failed");
	}
	freeaddrinfo(bind_addr);
	Webserver::add_socket(listen_sockets);
	return listen_sockets;
}

void Webserver::init_registry()
{
	std::vector<std::pair<std::string, std::string> >					host_port;

	for (size_t i = 0; i < _configs.size(); i++)
	{
		std::pair<std::string, std::string> tmp = std::make_pair(_configs[i].getHost(), _configs[i].getPort());
		if (find(host_port.begin(), host_port.end(), tmp) == host_port.end())
			host_port.insert(host_port.end(), tmp);
	}
	for (size_t i = 0; i < _configs.size(); i++)
	{
		for (size_t j = i + 1; j < _configs.size(); j++)
		{
			if ((_configs[i].getHost() == _configs[j].getHost()) && \
				(_configs[i].getPort() == _configs[j].getPort()) && \
				(_configs[i].getServerNames() == _configs[j].getServerNames()))
					throw CustomeExceptionMsg("it's forbidden to set two identical server, at least change the server name -_-");

		}
	}
	for (size_t i = 0; i < host_port.size(); i++)
	{
		try
		{
			SOCKET tmp = server_socket(host_port[i].first, host_port[i].second);
			_registry.insert(_registry.end(), Registry(host_port[i].first, host_port[i].second, tmp));
		}
		catch(const ServerException& e)
		{
			std::cerr << "WARNING " << e.what() << " " << host_port[i].first << ":" << host_port[i].second << std::endl;
		}
	}
}

Webserver::Webserver(std::string content): _configs(init_configs(content))
{
	this->init_registry();
}

void Webserver::add_socket(SOCKET socket)
{
	FD_SET(socket, &_readset);
	if (_max_socket < socket)
		_max_socket = socket;
}

ConfVec  Webserver::init_configs(std::string content)
{
	ConfVec 	configs;
	TokenVects 									data(SplitValues(content));
	std::pair<TokenVectsIter, TokenVectsIter>	it(std::make_pair(data.begin(),  data.end()));

	while (it.first < it.second)
	{
		if (it.first->second == BLOCK)
		{
			string_trim(*(it.first));
			if (it.first->first == "server")
				configs.insert(configs.end(), Configuration(it.first, it.second));
			else
				throw CustomeExceptionMsg(it.first->first + BlockErro);
		}
		else if ((it.first->second != END))
			throw CustomeExceptionMsg(it.first->first + BlockErro);
		it.first++;
	}
	if (configs.empty())
		throw CustomeExceptionMsg(EmptyFile);
	return configs;
}


Webserver::Webserver(const Webserver&  other)
{
	*this = other;
}

Webserver& Webserver::operator=(const Webserver&  other)
{
	_configs = other._configs;
	_registry = other._registry;
	_readset = other._readset;
	_max_socket = other._max_socket;
	return *this;
}


bool Webserver::wait_on_client(SetsPair& sets)
{
	sets = std::make_pair(_readset, _writeset);
	if (select(_max_socket + 1, &sets.first, &sets.second, 0, 0) < 0)
	{
		this->reset();
		return false;
	}
	return true;
}

void Webserver::reset()
{
	FD_ZERO(&_readset);
	FD_ZERO(&_writeset);
	for (size_t i = _registry.size(); i != _registry.size(); i++)
	{
		FD_SET(_registry[i]._listen_socket, &_readset);
		for (size_t i = 0; i < _clients.size(); i++)
			this->drop_client(i);
	}
}

void  Webserver::clear_set()
{
	for (SOCKET fd = 0; fd <= Webserver::_max_socket; fd++) {
		if (FD_ISSET(fd, &Webserver::_readset) || FD_ISSET(fd, &Webserver::_writeset))
		{
			close(fd);
		}
	}
	FD_ZERO(&_readset);
	FD_ZERO(&_writeset);
}

int Webserver::fetch_request (Client *client)
{
	char request[MAX_REQUEST_SIZE + 1] = {0};
	int r;

	r = recv(client->_socket, request, MAX_REQUEST_SIZE, 0);
	if (r < 1)
	{
		std::cerr << "Unexpected disconnect from " << client->get_client_address() << std::endl;
		FD_CLR(client->_socket, &_readset);
		return 0;
	}
	else
	{
		request[r] = '\0';
		client->_request.parseRequest(request, client->_registry, this->_configs, r);
		if (client->_request.getChunkedState() == DONE)
		{
			FD_CLR(client->_socket, &_readset);
			FD_SET(client->_socket, &_writeset);
		}
	}
	return 1;
}

void Webserver::drop_client(size_t i)
{
	if (i < _clients.size())
	{
		delete _clients[i];
		_clients.erase(_clients.begin() + i);
	}
}

const std::vector<Registry>& Webserver::get_registry() const
{
	return this->_registry;
}

void Webserver::run()
{
    SetsPair temps;

	while (1)
    {
		try
		{
			if (!wait_on_client(temps))
				continue;
			for (size_t i = 0; i < _registry.size(); i++)
			{
				if (FD_ISSET(_registry[i]._listen_socket, &temps.first))
				{
					try
					{
						_clients.insert(_clients.end(), new Client(_registry[i]));
					}
					catch(const Client::ClientException& e)
					{
						std::cerr << "WARNING : "<< e.what() << std::endl;
					}
				}
			}
			for (size_t i = 0; i < _clients.size(); i++)
			{
				if (FD_ISSET(_clients[i]->_socket, &temps.first))
				{
					if (!fetch_request(_clients[i])) {
						this->drop_client(i); continue ; }
				}
				if (FD_ISSET(_clients[i]->_socket, &temps.second))
				{
					if(send_response(_clients[i]))
						this->drop_client(i);
				}
			}
		}
		catch(const WebserverReset& e)
		{
			std::cerr << e.what() << "\nrestarting the server\n.\n..\n...\n...\n.....\n........." << std::endl;
			this->reset();
		}
    }
}

int Webserver::send_response(Client *client)
{
	if (!client->_remaining)
	{
		client->_response.serveResponse(client->_request);
		if (client->_response.getIsHeaderParsed() && !client->_response.getIsHeaderSent())
			client->_data_sent = client->_response.sendHeader();
		else
			client->_data_sent = client->_response.getBody();
	}
	client->_bytesSent = send(client->_socket, client->_data_sent.c_str(), client->_data_sent.size(), 0);
	if (client->_response.getIsBodySent() || client->_bytesSent == -1)
	{
		if(client->_response.getIsBodySent())
			std::cout << "data sent succefully" << std::endl;
		else
			std::cerr << "send system call fail" << std::endl;
		FD_CLR(client->_socket, &_writeset);
		return 1;
	}
	if (client->_bytesSent == (ssize_t)client->_data_sent.size())
		client->_remaining = false;
	else if (client->_bytesSent < (ssize_t)client->_data_sent.size())
	{
		client->_remaining = true;
		client->_data_sent = client->_data_sent.substr(client->_bytesSent, client->_data_sent.length());
	}
	return 0;
}

void Webserver::stop()
{
	for (size_t i = 0; i != _registry.size(); i++)
		close(_registry[i]._listen_socket);
}


Webserver::WebserverReset::WebserverReset() : CustomeExceptionMsg()
{}

Webserver::WebserverReset::WebserverReset(const std::string& message) : CustomeExceptionMsg(message)
{}

Webserver::WebserverReset::~WebserverReset() throw()
{}


Webserver::~Webserver()
{
	this->stop();
}
