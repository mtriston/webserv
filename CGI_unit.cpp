#include "CGI_unit.hpp"
#include <iostream>

bool CGI_unit::_read_content_len_check(void)
{
	if (_cgi_out_len < _answer.size() - _cgi_out_body_pos - 5)
		return false;
	return true;
}

void CGI_unit::_read_content_len_fill(void)
{
	int cont_len_pos;
	
	_cgi_out_body_pos = _answer.find("\r\n\r\n");
	cont_len_pos = _answer.find("Content-Length:");
	if (cont_len_pos < _cgi_out_body_pos)
		_cgi_out_len = atoi(&_answer[cont_len_pos + 15]);
}

int CGI_unit::_cgi_read(void)
{
	char	buf[8193];
	int		len = 8192;
	int		status;
	int		wp;
	
	wp = waitpid(_pid, &status, WNOHANG);
	if (wp == 0)
		return pipes[0];
	len = read(pipes[0], buf, 8192);
	buf[len] = '\0';
	_answer.append(buf);
	if (len == 8192)
		return pipes[0];
	if (!WIFEXITED(status))
		return -1;
	if (_cgi_out_len == -1)
		_read_content_len_fill();
	else if (!_read_content_len_check())
		return pipes[0];
	close(pipes[0]);
	_status = CGI_DONE;
	return 0;
}

bool CGI_unit::_check_path(std::string const &path)
{
	int cnt;
	char const *str;
	int up;

	up = 0;
	cnt = 0;
	str = path.c_str();
	while (str[cnt] != '\0' && up > -1) {
		if (str[cnt] == '.' && str[cnt + 1] == '.' && \
                        str[cnt - 1] == '/')
			--up;
		if (str[cnt] == '/')
			++up;
		if (cnt > 3 && str[cnt] == '/' && str[cnt - 1] == '.'\
 && str[cnt - 2] == '.' && str[cnt - 3] == '/')
			--up;
		++cnt;
	}
	if (up < 0) 
	{
		_status = CGI_DONE;
//		_answer = to do pub и выводить коды ошибки 200 403 404
		return false;
	}
	return true;
}

bool CGI_unit::_check_file(std::string const &file)
{
	struct stat stats;

	if (!_check_path(file))
		return false;
	if (stat(file.c_str(), &stats)) {
		_status = CGI_DONE;
//		_answer = to do error 404
		return false;
	}
	return (true);
}

int CGI_unit::_cgi_write(void)
{
	int len;

	len = write(_post_pipes[1], &_body[_body_pos], _body_len);
	_body_pos += len;
	if (_body_pos < _body_len)
		return _post_pipes[1];
	close(_post_pipes[1]);
	_status = POST;
	return pipes[0];
}

int CGI_unit::_fork_get(cgi_preform &_env, char const **out_args)
{	
	fcntl(pipes[0], F_SETFL, O_NONBLOCK);
	_pid = fork();
	if (_pid == 0)
	{
		close(pipes[0]);
		dup2(pipes[1], 1);
		execve(out_args[0], const_cast<char *const *>(out_args), \
							const_cast<char *const *>(_env._env));
		exit(1);
	}
	close(pipes[1]);
	_status = GET;
	return pipes[0];
}

int CGI_unit::_fork_post(cgi_preform &_env, char const **out_args)
{	
	pipe(_post_pipes);
	fcntl(pipes[0], F_SETFL, O_NONBLOCK);
	_pid = fork();
	if (_pid == 0)
	{
		close(pipes[0]);
		dup2(pipes[1], 1);
		
		close(_post_pipes[1]);
		dup2(_post_pipes[0], 0);
		execve(out_args[0], const_cast<char *const *>(out_args), \
							const_cast<char *const *>(_env._env));
		exit(1);
	}
	close(pipes[1]);
	close(_post_pipes[0]);
	_status = POST_FORKED;
	return _post_pipes[1];
}

CGI_unit::cgi_preform::cgi_preform()
{
	_auth_type = "AUTH_TYPE=";
	_content_len =  "CONTENT_LENGTH=";
	_content_type = "CONTENT_TYPE=";
	_gateway_int = "GATEWAY_INTERFACE=CGI/1.1";
	_path_info = "PATH_INFO=";
	_path_trans = "PATH_TRANSLATED=";
	_query_string = "QUERY_STRING=";
	_remote_addr = "REMOTE_ADDR=";
	_remote_ident = "REMOTE_IDENT=";
	_remotw_user = "REMOTE_USER=";
	_request_method = "REQUEST_METHOD=";
	_request_uri = "REQUEST_URI=";
	_script_name = "SCRIPT_NAME=";
	_server_name = "SERVER_NAME=";
	_server_port = "SERVER_PORT=";
	_server_prot = "SERVER_PROTOCOL=HTTP/1.1";
	_server_soft = "SERVER_SOFTWARE=ft_webserv";
	_http_accept = "HTTP_ACCEPT=";
	_http_referer = "HTTP_REFERER=";
	_http_user_agent = "HTTP_USER_AGENT=";
	_http_accept_encoding = "HTTP_ACCEPT_ENCODING=";
	_http_accept_language = "HTTP_ACCEPT_LANGUAGE=";
	//to do cookie
}

void CGI_unit::cgi_preform::refilling(void)
{
	_env[0] = _auth_type.c_str();
	_env[1] = _content_len.c_str();
	_env[2] = _content_type.c_str();
	_env[3] = _gateway_int.c_str();
	_env[4] = _path_info.c_str();
	_env[5] = _path_trans.c_str();
	_env[6] = _query_string.c_str();
	_env[7] = _remote_addr.c_str();
	_env[8] = _remote_ident.c_str();
	_env[9] = _remotw_user.c_str();
	_env[10] = _request_method.c_str();
	_env[11] = _request_uri.c_str();
	_env[12] = _script_name.c_str();
	_env[13] = _server_name.c_str();
	_env[14] = _server_port.c_str();
	_env[15] = _server_prot.c_str();
	_env[16] = _server_soft.c_str();
	_env[17] = _http_accept.c_str();
	_env[18] = _http_referer.c_str();
	_env[19] = _http_user_agent.c_str();
	_env[20] = _http_accept_encoding.c_str();
	_env[21] = _http_accept_language.c_str();
	//to do cookie
	_env[22] = NULL;
}

void CGI_unit::_getEnv(cgi_preform &e, Request &r)
{
/*	e._auth_type.append(r.);
	e._content_len.append(r.);
	e._content_type.append(r.);
	e._path_info.append(r.);
	e._path_trans.append(r.);
	e._query_string.append(r.);
	e._remote_addr.append(r.);
	e._remote_ident.append(r.);
	e._remotw_user.append(r.);
	e._request_method.append(r.getMethod());
	e._request_uri.append(r.);
	e._script_name.append(r.);
	e._script_name.append(r.);
	e._server_name.append(r.);
	e._http_accept.append(r.);
	e._http_referer.append(r.);
	e._http_user_agent.append(r.);
	e._http_accept_encoding.append(r.);
	e._http_accept_language.append(r.);
	//to do cookie
*/
	e.refilling();
}

CGI_unit::CGI_unit(void){
	_body_pos = 0;
	_cgi_out_len = -1;
}
CGI_unit::~CGI_unit(void){}

int CGI_unit::work(void)
{
	int status;
	int wp;

	if (_status == GET || _status == POST)
		return _cgi_read();
	else if (_status == POST_FORKED)
		return _cgi_write();
	else
		return 0;
}


void CGI_unit::_checkType(std::string &name, char const**out_args)
{
	int len;
	
	len = name.size() - 1;
	if (name[len] == 'y' && name[len - 1] == 'p' && name[len - 2] == '.')
	{
		out_args[0] = _python_location.c_str();
		out_args[1] = name.c_str();
	}
	else if (name[len] == 'p' && name[len - 1] == 'h' && name[len - 2] == 'p'\
		&& name[len - 3] == '.')
	{
		out_args[0] = _php_location.c_str();
		out_args[1] = name.c_str();
	}
	else
	{
		out_args[0] = name.c_str();
		out_args[1] = NULL;
	}
	out_args[2] = NULL;
}

bool CGI_unit::checkRead(void)
{
	if (_status == GET || _status == POST)
		return true;
	return false;
}

bool CGI_unit::checkWrite(void)
{
	if (_status == POST_FORKED)
		return true;
	return false;
}

bool CGI_unit::checkDone(void)
{
	if (_status == CGI_DONE)
		return true;
	return false;
}

int CGI_unit::init(Request &res, int port, std::string name)
{
	cgi_preform _env;
	char const*	out_args[3];
	
	_env._server_port.append(std::to_string(port));
	_getEnv(_env, res);
	_body = res.getBody().c_str();
	_body_len = res.getBody().size();
	pipe(pipes);
	if (!_check_file(name))
		return pipes[0];
	_checkType(name, out_args);
	if (res.getMethod() == "GET")
		return _fork_get(_env, out_args);
	else
		return _fork_post(_env, out_args);
} 

std::string CGI_unit::getAnswer(void)
{
	return _answer;
};

std::string const &CGI_unit::Answer(void)
{
	return _answer;
};

void CGI_unit::setPythonLoc(std::string const & loc)
{
	_python_location = loc;
};

void CGI_unit::setPhpLoc(std::string const & loc)
{
	_php_location = loc;
};
