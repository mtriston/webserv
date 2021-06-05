//
// Created by mtriston on 04.05.2021.
//

#ifndef WEBSERV__RESPONSE_HPP_
#define WEBSERV__RESPONSE_HPP_

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include "utils.hpp"
#include "Request.hpp"

class ConnectionSocket;
class ConfigUnit;

struct response_data {
public:
    int fd;
    std::string file;
    std::string content;
    int contentLength;
    std::string contentType;
    std::string lastModified;
    int status;
};

enum response_states {
    PREPARE_FOR_GENERATE,
    READ_FILE,
    READ_CGI,
    WRITE_FILE,
    READY_FOR_SEND
};

class Response {
public:
    Response(ConnectionSocket *);

    ~Response();

    int fillFdSet(fd_set *readfds, fd_set *writefds) const;

    void initGenerateResponse();

    void generateResponse();

    std::string getResponse() const;

    bool isReadyGenerate(fd_set *readfds, fd_set *writefds) const;

    bool isGenerated() const;

private:
    Response();

    Response(Response const &);

    Response &operator=(Response const &);

    void _handleMethodGET();

    void _handleMethodHEAD();

    void _openContent();

    void _writeContent();

    std::string getHeaders() const;

    std::string _getContentType(std::string const &);

    ConnectionSocket *socket;
    ConfigUnit *config;
    Request *request;
    struct response_data responseData_;
    response_states state_;
};

#endif //WEBSERV__RESPONSE_HPP_
