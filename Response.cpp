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
		: socket(socket), responseData_(), state_(PREPARE_FOR_GENERATE) {
	errorMap_[200] = "OK";
	errorMap_[204] = "No Content";
	errorMap_[301] = "Moved Permanently";
	errorMap_[307] = "Temporary Redirect";
	errorMap_[308] = "Permanent Redirect";
	errorMap_[400] = "Bad Request";
	errorMap_[403] = "Forbidden";
	errorMap_[404] = "Not Found";
	errorMap_[405] = "Method Not Allowed";
	errorMap_[413] = "Request Entity Too Large";
	errorMap_[500] = "Internal Server Error";
	errorMap_[501] = "Not Implemented";
}

Response::Response(Response const &) {}

Response::~Response()
{
	delete request;
}

void Response::initGenerateResponse()
{
	request = new Request();
	request->parseRequest(socket->getBuffer());

	config = socket->getConfig()->getServerConf(request->getHost(), socket->getPort());

	if(isPayloadTooLarge()) {
		return _handleInvalidRequest(RequestTooLarge);
	}
	if (!config->checkMethod(request->getMethod(), request->getPath())) {
	    return _handleInvalidRequest(MethodNotAllowed);
    }
    if (config->checkRedirect(request->getPath())) {
    	responseData_.status = config->getRedirectPath(request->getPath()).first;
    	responseData_.location = config->getRedirectPath(request->getPath()).second;
    	state_ = READY_FOR_SEND;
	    return;
    }
	if (request->getMethod() == "GET") {
		_handleMethodGET();
	} else if (request->getMethod() == "HEAD") {
		_handleMethodHEAD();
	} else if (request->getMethod() == "DELETE") {
			_handleMethodDELETE();
	} else {
		_handleInvalidRequest(NotImplemented);
	}
}

void Response::generateResponse()
{
	if (state_ == READ_FILE) {
		char buf[1024] = {};
		long ret = read(responseData_.fd, buf, 1024);
		if (ret < 0) {
			close(responseData_.fd);
			responseData_ = response_data();
			_handleInvalidRequest(InternalError);
		} else { responseData_.content.append(std::string(buf, 1024)); }
		if (ret < 1024 || responseData_.content.size() == responseData_.contentLength) {
			state_ = READY_FOR_SEND;
			close(responseData_.fd);
		}
	}
}

void Response::_handleMethodHEAD() {
	this->_handleMethodGET();
	if (state_ == READ_FILE) {
		close(responseData_.fd);
		state_ = READY_FOR_SEND;
	}
}

void Response::_handleMethodGET()
{
	responseData_.file = config->getServerPath(request->getPath());
	responseData_.status = OK;
	_openContent();
}

void Response::_handleMethodDELETE()
{
	responseData_.file = config->getServerPath(request->getPath());
	responseData_.status = NoContent;
	int ret = std::remove(responseData_.file.c_str());
	if (ret < 0) {
		if (errno == EACCES) {
			return _handleInvalidRequest(Forbidden);
		} else if (errno == ENOENT) {
			return _handleInvalidRequest(NotFound);
		}
	} else {
		state_ = READY_FOR_SEND;
	}
}

void Response::_handleInvalidRequest(int code)
{
	responseData_.file = config->searchError_page(code);
	responseData_.status = code;

	if (isFileExists(responseData_.file)) {
		_openContent();
	} else {
		responseData_.content = generateErrorPage(code);
		responseData_.contentLength = responseData_.content.size();
		responseData_.contentType = "text/html";
		state_ = READY_FOR_SEND;
	}
}

std::string Response::getResponse()
{
	return getHeaders() + responseData_.content;
}

bool Response::isGenerated() const
{
	return state_ == READY_FOR_SEND;
}

void Response::_openContent()
{
	responseData_.fd = open(responseData_.file.c_str(), O_RDONLY);
	if (responseData_.fd == -1) {
		if (errno == EACCES) {
			return _handleInvalidRequest(Forbidden);
		} else if (errno == ENOENT) {
			return _handleInvalidRequest(NotFound);
		}
	}
	struct stat info = {};

	stat(responseData_.file.c_str(), &info);

	responseData_.contentLength = info.st_size;
	responseData_.contentType = _getContentType(responseData_.file);
	responseData_.lastModified = convertTime(&info.st_mtime);

	fcntl(responseData_.fd, F_SETFL, O_NONBLOCK);
	state_ = READ_FILE;
}

std::string Response::_getContentType(const std::string &file)
{
	std::string endWith = file.substr(file.find_last_of('.') + 1);

	if (endWith == "html")
		return "text/html";
	if (endWith == "jpg")
		return "image/jpeg";
	if (endWith == "png")
		return "image/png";
	if (endWith == "css")
		return "text/css";
	if (endWith == "js")
		return "application/javascript";
	return "text/plain";
}

bool Response::isReadyGenerate(fd_set *readfds, fd_set *writefds) const
{
	if (FD_ISSET(responseData_.fd, writefds) ||
			FD_ISSET(responseData_.fd, readfds)) {
		return true;
	}
	return false;
}

int Response::fillFdSet(fd_set *readfds, fd_set *writefds) const
{
	if (state_ == READ_FILE || state_ == READ_CGI) {
		FD_SET(responseData_.fd, readfds);
		return responseData_.fd;
	} else if (state_ == WRITE_FILE) {
		FD_SET(responseData_.fd, writefds);
		return responseData_.fd;
	}
	return -1;
}

std::string Response::getHeaders()
{
	std::stringstream headers;
	time_t t;
	time(&t);

	headers << "HTTP/1.1 " << responseData_.status << " " << errorMap_[responseData_.status] << "\r\n";
	if (!responseData_.location.empty()) {
		headers << "Location: " << responseData_.location << "\r\n";
	}
	if (responseData_.status == MethodNotAllowed || responseData_.status == NotImplemented) {
		headers << "Allow: ";
		for (
				std::list<std::string>::const_iterator i = config->getMethods().begin();
				i != config->getMethods().end();
			) {
			headers << *i;
			if (++i != config->getMethods().end()) {
				headers << ", ";
			}
		}
		headers << "\r\n";
	}
	headers << "Date: " << convertTime(&t) << "\r\n";
	if (!responseData_.content.empty() || request->getMethod() == "HEAD") {
		headers << "Content-Type: " << responseData_.contentType << "\r\n";
		headers << "Content-Length: " << responseData_.contentLength << "\r\n";
		headers << "Last-Modified: " << responseData_.lastModified << "\r\n";
	}
	headers << "\r\n";
	return headers.str();
}

bool Response::isPayloadTooLarge() const
{
	size_t maxClientBody = config->getMax_client_body();

	return request->getContentLength() > maxClientBody ||
		request->getBody().size() > maxClientBody * 2;
}

bool Response::isFileExists(const std::string &path)
{
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}

std::string Response::generateErrorPage(int code)
{
	std::stringstream page;
	page << "<!DOCTYPE html>\n"
			"<html lang=\"en\">\n"
				"<head>\n"
				"    <meta charset=\"UTF-8\">\n"
				"    <title>" << errorMap_[code] << "</title>\n"
               "</head>\n"
               "<body>\n"
               "<h1>" << code << " " << errorMap_[code] << "</h1>\n"
               "</body>\n"
           "</html>";
	return page.str();
}
