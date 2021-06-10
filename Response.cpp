#include <sys/stat.h>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <dirent.h>

#include "Response.hpp"
#include "ConnectionSocket.hpp"
#include "Config_unit.hpp"
#include "Config_parser.hpp"
#include "CGI_unit.hpp"
#include "utils.hpp"

Response::Response() {}

Response::Response(ConnectionSocket *socket)
		: socket(socket), responseData_(), state_(PREPARE_FOR_GENERATE)
{
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
	delete cgi;
}

void Response::initGenerateResponse()
{
	request = new Request();
	request->parseRequest(socket->getBuffer());

	config = socket->getConfig()->getServerConf(request->getHost(), socket->getPort());


	cgi = new CGI_unit();
	cgi->setPhpLoc(config->getPHPexec());
	cgi->setPythonLoc(config->getPythonExec());

	std::cout << request->getVersion() << std::endl;
	std::cout << request->getPath() << std::endl;
	std::cout << request->getMethod() << std::endl;
	std::cout << request->getBody() << std::endl;
	std::cout << request->getContentLength() << std::endl;
	std::cout << request->getHost() << std::endl;
	std::cout << request->getAccept() << std::endl;
	std::cout << request->getAuthType() << std::endl;
	std::cout << request->getContentType() << std::endl;
	std::cout << request->getCookies() << std::endl;
	std::cout << request->getQueryString() << std::endl;
	std::cout << request->getReferer() << std::endl;
	std::cout << request->getUserAgent() << std::endl;

	if (isPayloadTooLarge()) {
		return _handleInvalidRequest(RequestTooLarge);
	}
	if (!config->checkMethod(request->getMethod(), request->getPath())) {
		return _handleInvalidRequest(MethodNotAllowed);
	}
	if (config->checkRedirect(request->getPath())) {
		return _handleRedirect();
	}
	if (request->getMethod() == "GET") {
		_handleMethodGET();
	} else if (request->getMethod() == "HEAD") {
		_handleMethodHEAD();
	} else if (request->getMethod() == "POST") {
		_handleMethodPOST();
	} else if (request->getMethod() == "DELETE") {
		_handleMethodDELETE();
	} else {
		_handleInvalidRequest(NotImplemented);
	}
}

void Response::generateResponse()
{
	if (state_ == READ_FILE) {
		_readContent();
	} else if (state_ == WRITE_FILE) {
		_writeContent();
	} else if (state_ == PROCESSING_CGI) {
		_processCGI();
	}
}

void Response::_handleMethodHEAD()
{
	this->_handleMethodGET();
	if (state_ == READ_FILE) {
		close(responseData_.fd);
		state_ = READY_FOR_SEND;
	}
}

void Response::_handleMethodGET()
{
	responseData_.status = OK;
	responseData_.file = config->getPathFromLocation(request->getPath());

	if (isCGI(responseData_.file)) {
      responseData_.fd = cgi->init(*request, socket->getPort(), config->getCGI_Path(request->getPath()));
		state_ = PROCESSING_CGI;
	} else if (isAutoIndex()) {
			responseData_.content = getDirListing(responseData_.file, request->getPath());
			responseData_.contentLength = responseData_.content.size();
			responseData_.contentType = "text/html";
			state_ = READY_FOR_SEND;
	} else {
		responseData_.file = config->getServerPath(request->getPath());
		_openContent();
	}
}

void Response::_handleMethodPOST()
{
	responseData_.file = config->getUploadPath(request->getPath());
	responseData_.fd = open(responseData_.file.c_str(),
	                        O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
	fcntl(responseData_.fd, F_SETFL, O_NONBLOCK);
	if (responseData_.fd < 0) {
		return _handleInvalidRequest(Forbidden);
	}
	responseData_.content = request->getBody();
	state_ = WRITE_FILE;
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
	if (isDirectory(responseData_.file))
		return _handleInvalidRequest(Forbidden);
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

void Response::_readContent()
{
	char buf[BUF_SIZE] = {};
	long ret = read(responseData_.fd, buf, BUF_SIZE);
	if (ret < 0) {
		close(responseData_.fd);
		responseData_ = response_data();
		return _handleInvalidRequest(InternalError);
	} else {
		responseData_.content.append(std::string(buf, BUF_SIZE));
	}
	if (ret < BUF_SIZE || responseData_.content.size() == responseData_.contentLength) {
		state_ = READY_FOR_SEND;
		close(responseData_.fd);
	}
}

void Response::_writeContent()
{
	long ret = write(responseData_.fd, responseData_.content.c_str(), responseData_.content.size());
	if (ret < 0) {
		close(responseData_.fd);
		responseData_ = response_data();
		return _handleInvalidRequest(InternalError);
	} else {
		responseData_.content.erase(0, ret);
		if (responseData_.content.empty()) {
			close(responseData_.fd);
			responseData_.status = 204;
			state_ = READY_FOR_SEND;
		}
	}
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
	if (state_ == READ_FILE || cgi->checkRead()) {
		FD_SET(responseData_.fd, readfds);
		return responseData_.fd;
	} else if (state_ == WRITE_FILE || cgi->checkWrite()) {
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
		for (std::list<std::string>::const_iterator i = config->getMethods().begin();
			i != config->getMethods().end();)
		{
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

std::string Response::getDirListing(std::string const &path, std::string const &req)
{
	DIR            *folder;
	FILE_INFO        *file;
	std::string        page;

	folder = opendir(path.c_str());
	if (folder == 0)
		return (page);
	file = readdir(folder);
	page.append(
			"<html>\n"
				"<head><title>Index of " + req + "</title></head>\n"
                "<body bgcolor=\"white\">\n"
	       "        <h1>Index of "
"                       <a href=\"" + req + "\">" + req + "</a></h1>");
	while (file != 0)
	{
		page.append("<a href = \"");
		page.append(req);
		page.append(file->d_name);
		if (file->d_type == 4)
			page.append("/");
		page.append("\">");
		page.append(file->d_name);
		if (file->d_type == 4)
			page.append("/");
		page.append("</a><br>");
		file = readdir(folder);
	}
	closedir(folder);
	page.append("</body>");
	return page;
}

bool Response::isAutoIndex()
{
	bool flag = config->checkAutoindex(request->getPath());
	bool dir = isDirectory(config->getPathFromLocation(request->getPath()));
	return  flag && dir;
}

void Response::_handleRedirect()
{
	responseData_.status = config->getRedirectPath(request->getPath()).first;
	responseData_.location = config->getRedirectPath(request->getPath()).second;
	state_ = READY_FOR_SEND;
}

bool Response::isCGI(const std::string &path)
{
	return true;
}

void Response::_processCGI()
{
	int ret = cgi->work();
	std::string buf = cgi->Answer();
	if (ret < -1) {
		_handleInvalidRequest(500);
	} else if (ret > 0) {
		bool flag = cgi->checkRead();
		responseData_.fd = ret;
	} else if (cgi->checkDone()) {
		responseData_.content = cgi->getAnswer();
		state_ = READY_FOR_SEND;
	}
}
