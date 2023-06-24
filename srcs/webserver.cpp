#include "../includes/server.hpp"

fd_set Webserver::_readset;
fd_set Webserver::_writeset;
SOCKET Webserver::_max_socket = 0;

Webserver::Webserver() 
// : _servers()
{

}


bool compare(Configuration a, Configuration b)
{
	if((a.getHost() == b.getHost()) && a.getPort() == b.getPort())
		return true;
	return false;
}

// int setnonblocking(int sock)
// {
// 	int opts;

// 	opts = fcntl(sock,F_GETFL);
// 	(opts < 0) ? ret : (NULL);
// 	opts = (opts | O_NONBLOCK);
// 	(fcntl(sock,F_SETFL,opts) < 0) ? \
// 	throw ServerException("fcntl failed to set set the modified file status flags back to the socket") : (NULL);
// }


SOCKET server_socket(std::string host, std::string port)
{	
	int opt;
	SOCKET listen_sockets;
	s_addrinfo hints;
    s_addrinfo *bind_addr;
	std::string error_message("");

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(host.c_str(), port.c_str(), &hints, &bind_addr);
	if (!bind_addr)
		error_message = "getaddrinfo system call failed.";
	else
	{
		listen_sockets = socket(bind_addr->ai_family, bind_addr->ai_socktype, bind_addr->ai_protocol);
		if (listen_sockets < 0)
			error_message = "socket system call failed.";
		setsockopt(listen_sockets, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		if (fcntl(listen_sockets, F_SETFL, fcntl(listen_sockets, F_GETFL, 0) | O_NONBLOCK) == -1) {
			freeaddrinfo(bind_addr);
			close(listen_sockets);
        	return -1;
		}
		// setnonblocking(listen_sockets);
		if (bind(listen_sockets, bind_addr->ai_addr, bind_addr->ai_addrlen))
		{ close(listen_sockets); error_message = "bind system call failed." + std::string(strerror(errno)); }
		if (listen(listen_sockets, SOMAXCONN) < 0)
		{ close(listen_sockets); error_message = "listen system call failed."; }
	}
	freeaddrinfo(bind_addr);
	if (!error_message.empty())
		return -1;
	return listen_sockets;
}

void Webserver::get_registry()
{
	std::map<std::string, std::string>						_host_port_map;
	typedef std::map<std::string, std::string>::iterator	iterator;

	for (size_t i = 0; i < _configs.size(); i++)
		_host_port_map[_configs[i].getHost()] = _configs[i].getPort();
	for (iterator it = _host_port_map.begin(); it != _host_port_map.end(); it++)
	{
		SOCKET tmp = server_socket(it->first, it->second);
		if (tmp == -1)
			std::cout << "socket setuping failed for " << it->first << ":" << it->second << std::endl;
		else
		{
			Webserver::add_socket(tmp);
			_registry.insert(_registry.end(), Registry(it->first, it->second, tmp));
		}
	}
}

Webserver::Webserver(std::string content): _configs(init_configs(content))
{
	this->get_registry();
}

void Webserver::add_socket(SOCKET socket)
{
	FD_SET(socket, &_readset);
	if (_max_socket < socket)
		_max_socket = socket;
}


// check_duplicate_conf(Webserver::ConfVec configs); // chi compare;

Webserver::ConfVec  Webserver::init_configs(std::string content)
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
	// check_duplicate_conf(Webserver::ConfVec configs);
	return configs;
}


Webserver::Webserver(const Webserver&  other)
{
	*this = other;
}

Webserver& Webserver::operator=(const Webserver&  other)
{
	_configs = other._configs;
	// _servers = other._servers;
	// _readset = other._readset;
	// _max_socket = other._max_socket;
	return *this;
}


bool Webserver::wait_on_client(SetsPair& sets)
{
	struct timeval timeout;

	timeout.tv_sec = 0;
	timeout.tv_usec = 5;
	sets = std::make_pair(_readset, _writeset);
	if (select(_max_socket + 1, &sets.first, &sets.second, 0, &timeout) < 0)\
	{
		// this->reset();
		return false;
	}
	return true;
}

// void Webserver::reset()
// {
// 	FD_ZERO(&_readset);
// 	FD_ZERO(&_writeset);
// 	for (ServerVec::iterator it = _servers.begin(); it != _servers.end(); it++)
// 	{
// 		FD_SET((*it)->get_listen_sockets(), &_readset);
// 		size_t clients_num = (*it)->get_clients().size();
// 		for (size_t i = 0; i < clients_num; i++)
// 			(*it)->drop_client(i);
// 	}
// }

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
		std::cout << "Unexpected disconnect from " << client->get_client_address() << strerror(errno) << std::endl;
		FD_CLR(client->_socket, &_readset);
		return 0;
	}
	else
	{
		request[r] = '\0';
		// client->_request.parseRequest(request, conf, r);
		if (client->_request.getChunkedState() == DONE)
		{
			FD_CLR(client->_socket, &_readset);
			FD_SET(client->_socket, &_writeset);
		}
	}
	return 1;
}

void Webserver::run()
{
    SetsPair temps;

	while (1)
    {
		// try
		// {
			if (!wait_on_client(temps))
				continue;
			for (size_t i = 0; i < _registry.size(); i++)
			{
				if (FD_ISSET(_registry[i]._listen_socket, &temps.first))
					_clients.insert(_clients.end(), new Client(_registry[i]));
			}
			for (size_t i = 0; i < _clients.size(); i++)
			{
				if (FD_ISSET(_clients[i]->_socket, &temps.first))
				{
					this->fetch_request(_clients[i]);
					// if (!fetch_request(_client[i])) {
					// 	(*it)->drop_client(i); continue ; }
				}
			}
        	// for (ServerVec::iterator it = _servers.begin(); it != _servers.end(); it++)
        	// {
            // 	std::vector<Client *>& _client = (*it)->get_clients();
    	    // 	if (FD_ISSET((*it)->get_listen_sockets(), &temps.first))
		    // 		_client.insert(_client.end(), new Client((*it)->get_listen_sockets()));
		   	// 	for (size_t i = 0; i < _client.size(); i++)
		    // 	{
		    // 		if (FD_ISSET(_client[i]->_socket, &temps.first))
		    // 		{
			// 			if (!fetch_request(_client[i], (*it)->get_configuration())) {
			// 				(*it)->drop_client(i); continue ; }
			// 		}
			// 		if (FD_ISSET(_client[i]->_socket, &temps.second))
			// 		{
			// 			if(send_response(_client[i]))
			// 				(*it)->drop_client(i);
			// 		}
		   	// 	}
        	// }
		// }
		// catch(const std::exception& e)
		// {
			// std::cerr << e.what() << std::endl;
			// std::cout << "restarting the server\n.\n..\n...\n...\n.....\n........." << std::endl;
			// this->reset();
		// }
    }
	// this->stop();
}

// int Webserver::send_response(Client *client)
// {
// 	if (!client->_remaining)
// 	{
// 		client->_response.serveResponse(client->_request);
// 		if (client->_response.getIsHeaderParsed() && !client->_response.getIsHeaderSent())
// 			client->_data_sent = client->_response.sendHeader();
// 		else
// 			client->_data_sent = client->_response.getBody();
// 	}
// 	client->_bytesSent = send(client->_socket, client->_data_sent.c_str(), client->_data_sent.size(), 0);
// 	if (client->_bytesSent == (ssize_t)client->_data_sent.size())
// 	{
// 		std::cout << "send all" << std::endl;
// 		client->_remaining = false;
// 	}
// 	else if (client->_bytesSent < (ssize_t)client->_data_sent.size())
// 	{
// 		client->_remaining = true;
// 		if (client->_bytesSent != -1)
// 		{
// 			std::cout << "_remaining " << client->_bytesSent << std::endl;
// 			client->_data_sent = client->_data_sent.substr(client->_bytesSent, client->_data_sent.length());
// 		}
// 	}
// 	if (client->_response.getIsBodySent() || client->_bytesSent == -1) 
// 	{
// 		std::cout << "***************************************** disconnected **************************************************" << std::endl;
// 		FD_CLR(client->_socket, &_writeset);
// 		return 1;
// 	}
// 	return 0;
// }

// void Webserver::stop()
// {
// 	for (ServerVec::iterator it = _servers.begin(); it != _servers.end(); it++)
// 	{
// 		close((*it)->get_listen_sockets());
// 	}
// }


Webserver::WebserverReset::WebserverReset() : CustomeExceptionMsg()
{}

Webserver::WebserverReset::WebserverReset(const std::string& message) : CustomeExceptionMsg(message)
{}

Webserver::WebserverReset::~WebserverReset() throw()
{}


Webserver::~Webserver()
{
    // for (ServerVec::iterator it = _servers.begin(); it != _servers.end(); it++)
    // {
    //     delete (*it);
    // }
}
