/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: waboutzo <waboutzo@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/23 16:35:07 by waboutzo          #+#    #+#             */
/*   Updated: 2023/05/27 17:07:05 by waboutzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Main.hpp"
//base_class


// //	child class

class CommonEntity
{
	protected:
		std::string											_root;
		std::vector<std::string> 							_index;
		std::vector<s_err_pages> 	 						_error_pages;
		size_t 												_client_max_body_size;
		bool 												_AutoIndex;
	public:
		CommonEntity();
		CommonEntity(TokenVectsIter begin, TokenVectsIter end);
		CommonEntity(const CommonEntity& other);
		CommonEntity& operator=(const CommonEntity& other);
		void InitRoot(std::string value);
		void InitIndex(std::string value);
		void InitErrorPage(std::string value);
		void InitClienBodySize(std::string value);
		void InitAutoIndex(std::string value);
		std::string					getRoot() const;
		std::vector<std::string> 	getIndex() const;
		std::vector<s_err_pages> 	getErrorPages() const;
		size_t 						getClientMaxBodySize() const;
		bool 						getAutoIndex() const;
};

class Location : public CommonEntity
{
	private:
		std::string _pattren;
		std::vector<std::string> _limit_except;
		std::vector<s_cgi> _cgi;
		std::string _upload;
		std::vector<std::string> _redirect;
	public:
		Location();
		Location(const CommonEntity& base, TokenVectsIter& begin, TokenVectsIter& end);
		void InitPattern(std::string value);
		void InitLimitExcept(std::string value);
		void InitCgi(std::string value);
		void InitUpload(std::string value);
		void InitRedirect(std::string value);
		std::string getPattren() const;
		std::vector<std::string> getLimit_except() const;
		std::vector<s_cgi> getCgi() const;
		std::string getUpload() const;
		std::vector<std::string> getRedirect() const;
		friend std::ostream& operator<<(std::ostream& o, Location obj);
		~Location();
};

class Server
{
	private:
		std::string 										_host;
		std::vector<int> 									_ports;
		std::vector<std::string>							_server_name;
		std::vector<Location>								_locations;
	public:
		Server();
		Server(TokenVectsIter& begin, TokenVectsIter& end);
		void initAttributes(TokenVectsIter& begin, TokenVectsIter& end);
        Server(const Server& other);
		Server& operator=(const Server& other);
		void InitHost(std::string value);
		void InitPort(std::string value);
		void InitServerName(std::string value);
		std::string 				getHost() const;
		std::vector<int> 			getPorts() const;
		std::vector<std::string>	getServerNames() const;
		std::vector<Location>		getLocations() const;
		friend std::ostream& operator<<(std::ostream& o, Server obj);
		~Server();
};


typedef void (CommonEntity::*CommonEntityMethods)(std::string);
typedef void (Location::*LocationMethods)(std::string);
typedef void (Server::*initserver)(std::string);