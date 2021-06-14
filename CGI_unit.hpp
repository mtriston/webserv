#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <sys/stat.h>
#include "Request.hpp"

/*
struct Request
{
	int read_fd;
	pid_t pid;	
	std::string body;
	std::string method;
	std::string const &getBody(void)
	{
		return (body);
	}
	std::string const &getMethod(void)
	{
		return (method);
	}
	
};
*/
class CGI_unit {

#define GET 10
#define POST 21
#define CGI_DONE 100
#define SENDED 31

	//env part
	struct cgi_preform {
		char const *_env[24];
		std::string _auth_type;
		std::string _content_len;
		std::string _content_type;
		std::string _gateway_int;
		std::string _path_info;
		std::string _path_trans;
		std::string _query_string;
		std::string _remote_addr;
		std::string _remote_ident;
		std::string _remotw_user;
		std::string _request_method;
		std::string _request_uri;
		std::string _script_name;
		std::string _server_name;
		std::string _server_port;
		std::string _server_prot;
		std::string _server_soft;
		std::string _http_accept;
		std::string _http_referer;
		std::string _http_user_agent;
		std::string _http_accept_encoding;
		std::string _http_accept_language;
		std::string _cookie;
		std::string _name;

		cgi_preform(void);

		void refilling(void);
	};

	//work part
	pid_t _pid;
	std::string _name;
	char const *_body;
	int _body_pos;
	int _body_len;
	int pipes[2];
	int _post_pipes[2];
	int _status;
	std::string _php_location;
	std::string _python_location;
	std::string _answer;
	int _cgi_out_len;
	bool _nph;

	int _fork(cgi_preform &, char const **);

	void _getEnv(cgi_preform &, Request &);

	void _checkType(std::string &, char const **);

	int _cgi_read(void);

	bool _check_path(std::string const &);

	int _cgi_write(bool);

	bool _read_content_len_check(void);

	void _all_headers(void);

	std::string _add_cont_len_header(int);

	bool _recheck_frst_ln(void);

	void _checkHeaders(void);

public:
	CGI_unit();

	~CGI_unit();

	int init(Request &, int, std::string);

	void setPythonLoc(std::string const &);

	void setPhpLoc(std::string const &);

	bool checkWrite(void);

	bool checkDone(void);

	bool checkRead(void);

	int check_file(std::string const &);

	int work(void);

	std::string getAnswer(void);

	std::string const &Answer(void);
};
