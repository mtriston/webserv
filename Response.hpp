//
// Created by mtriston on 04.05.2021.
//

#ifndef WEBSERV__RESPONSE_HPP_
#define WEBSERV__RESPONSE_HPP_

#include "Config.hpp"
#include "Request.hpp"

class Response {
	public:
		Response(Request const &request, Config const &config);
		Response(Response const &);
		std::string const &getResponse();
		~Response();

	private:
		Response();
		Request request_;
		Config config_;
		std::map<std::string, std::string> headers_;
		std::string response_;
};

#endif //WEBSERV__RESPONSE_HPP_
