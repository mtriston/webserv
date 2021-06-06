//
// Created by mtriston on 04.05.2021.
//

#ifndef WEBSERV__RESPONSE_HPP_
#define WEBSERV__RESPONSE_HPP_

#include <string>
#include <sys/types.h>
#include <map>

#define BUF_SIZE 2048
#define FILE_INFO struct dirent

class ConnectionSocket;

class config_unit;

class Request;

struct response_data {
public:
	int fd;
	std::string file;
	std::string content;
	size_t contentLength;
	std::string contentType;
	std::string lastModified;
	std::string location;
	int status;
};

enum response_states {
	PREPARE_FOR_GENERATE,
	READ_FILE,
	READ_CGI,
	WRITE_FILE,
	READY_FOR_SEND
};

enum code {
	OK = 200,
	NoContent = 204,
	MovedPermanently = 301,
	BadRequest = 400,
	Forbidden = 403,
	NotFound = 404,
	MethodNotAllowed = 405,
	RequestTooLarge = 413,
	InternalError = 500,
	NotImplemented = 501
};

class Response {
public:
	Response(ConnectionSocket *);

	~Response();

	int fillFdSet(fd_set *readfds, fd_set *writefds) const;

	void initGenerateResponse();

	void generateResponse();

	std::string getResponse();

	bool isReadyGenerate(fd_set *readfds, fd_set *writefds) const;

	bool isGenerated() const;

private:
	Response();

	Response(Response const &);

	Response &operator=(Response const &);

	static bool isDirectory(std::string const &path);

	bool isAutoIndex();

	std::string getDirectoryListing(std::string const &path, std::string const &request) const;

	void _handleMethodGET();

	void _handleMethodHEAD();

	void _handleMethodPOST();

	void _handleMethodDELETE();

	void _handleInvalidRequest(int code);

	static bool isFileExists(std::string const &path);

	std::string generateErrorPage(int code);

	void _openContent();

	std::string getHeaders();

	bool isPayloadTooLarge() const;

	static std::string _getContentType(std::string const &);

	ConnectionSocket *socket;
	config_unit *config;
	Request *request;
	struct response_data responseData_;
	std::map<int, std::string> errorMap_;
	response_states state_;
};

#endif //WEBSERV__RESPONSE_HPP_
