#include <sys/stat.h>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cstdio>

#include "Response.hpp"
#include "ConnectionSocket.hpp"
#include "Config_unit.hpp"
#include "Config_parser.hpp"
#include "utils.hpp"

Response::Response() {}

Response::Response(ConnectionSocket *socket)
        : socket(socket), responseData_(), state_(PREPARE_FOR_GENERATE) {}

Response::Response(Response const &) {}

Response::~Response() {
    delete request;
}

void Response::initGenerateResponse() {
    request = new Request();
    request->parseRequest(socket->getBuffer());

    config = socket->getConfig()->getServerConf(request->getHost(), socket->getPort());

//    if (!config->checkMethod(request->getMethod(), request->getPath())) {
//        _handleNotAllowedMethod();
//    } else
    if (request->getMethod() == "GET") {
        _handleMethodGET();
    } else if (request->getMethod() == "DELETE") {
        _handleMethodDELETE();
    } else {
        _handleNotAllowedMethod();
    }
}

void Response::generateResponse() {

    if (state_ == READ_FILE) {
        char buf[1024] = {};
        long ret = read(responseData_.fd, buf, 1024);
        if (ret < 0) {
            std::cerr << "read error" << std::endl;
        } else { responseData_.content.append(std::string(buf, 1024)); }
        if (ret < 1024 || responseData_.content.size() == responseData_.contentLength) {
            state_ = READY_FOR_SEND;
            close(responseData_.fd);
        }
    }
}

void Response::_handleMethodGET() {
    responseData_.file = config->getServerPath(request->getPath());
    responseData_.status = 200;
    _openContent();
    state_ = READ_FILE;
}

void Response::_handleMethodDELETE() {
    responseData_.file = config->getServerPath(request->getPath());
    responseData_.status = 204;
    int ret = std::remove(responseData_.file.c_str());
    if (ret < 0) {
        if (errno == EACCES) {
            responseData_.status = 403;
            responseData_.file = config->searchError_page(403);
        } else if (errno == ENOENT) {
            responseData_.status = 404;
            responseData_.file = config->searchError_page(404);
        }
        state_ = READ_FILE;
        _openContent();
    } else {
        state_ = READY_FOR_SEND;
    }
}

void Response::_handleNotAllowedMethod() {
    responseData_.file = config->searchError_page(405);
    responseData_.status = 405;
    _openContent();
    state_ = READ_FILE;
}


std::string Response::getResponse() const {
    return getHeaders() + responseData_.content;
}

bool Response::isGenerated() const {
    return state_ == READY_FOR_SEND;
}

void Response::_openContent() {

    responseData_.fd = open(responseData_.file.c_str(), O_RDONLY);
    if (responseData_.fd == -1) {
        if (errno == EACCES) {
            responseData_.status = 403;
            responseData_.file = config->searchError_page(403);
        } else if (errno == ENOENT) {
            responseData_.status = 404;
            responseData_.file = config->searchError_page(404);
        }
        responseData_.fd = open(responseData_.file.c_str(), O_RDONLY);
    }
    struct stat info = {};

    stat(responseData_.file.c_str(), &info);

    responseData_.contentLength = info.st_size;
    responseData_.contentType = _getContentType(responseData_.file);
    responseData_.lastModified = convertTime(&info.st_mtime);

    fcntl(responseData_.fd, F_SETFL, O_NONBLOCK);
}

std::string Response::_getContentType(const std::string &file) {

    if (file.size() > 5 && file.compare(file.size() - 5, 5, ".html") == 0) {
        return "text/html";
    } else if (file.size() > 4 && file.compare(file.size() - 4, 4, ".jpg") == 0) {
        return "image/jpeg";
    } else if (file.size() > 4 && file.compare(file.size() - 4, 4, ".png") == 0) {
        return "image/png";
    } else if (file.size() > 4 && file.compare(file.size() - 4, 4, ".css") == 0) {
        return "text/css";
    } else if (file.size() > 3 && file.compare(file.size() - 3, 3, ".js") == 0) {
        return "application/javascript";
    } else {
        return "text/plain";
    }
}

bool Response::isReadyGenerate(fd_set *readfds, fd_set *writefds) const {

    if (FD_ISSET(responseData_.fd, writefds) || FD_ISSET(responseData_.fd, readfds)) {
        return true;
    }
    return false;
}

int Response::fillFdSet(fd_set *readfds, fd_set *writefds) const {
    if (state_ == READ_FILE || state_ == READ_CGI) {
        FD_SET(responseData_.fd, readfds);
        return responseData_.fd;
    } else if (state_ == WRITE_FILE) {
        FD_SET(responseData_.fd, writefds);
        return responseData_.fd;
    }
    return -1;
}

std::string Response::getHeaders() const {
    std::stringstream headers;
    time_t t;
    time(&t);

    headers << "HTTP/1.1 " << responseData_.status << "\r\n";
    headers << "Date: " << convertTime(&t) << "\r\n";
    if (!responseData_.content.empty()) {
        headers << "Content-Type: " << responseData_.contentType << "\r\n";
        headers << "Content-Length: " << responseData_.contentLength << "\r\n";
        headers << "Last-Modified: " << responseData_.lastModified << "\r\n";
    }
    headers << "\r\n";
    return headers.str();
}
