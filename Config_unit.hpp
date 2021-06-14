#ifndef CONFIG_UNIT_H
#define CONFIG_UNIT_H

#include <string>
#include <map>
#include <list>
#include <cstring>
#include <sstream>

#include <iostream> //delete

struct listen_unit {
	std::string str;
	unsigned char digit[4];
	char type;
	int port;

	listen_unit &operator=(listen_unit const &);
};

struct location_unit {
	std::string _abs_path;
	std::string _def_file;
	std::list<std::string> _methods;
	std::string _storage;
	size_t _body_size;
	int _autoindex;
	std::pair<int, \
        std::string> _redirect;

	location_unit &operator=(location_unit const &);
};

class config_unit {

	std::list<std::string> _name;
	std::map<std::string, \
            location_unit> _location;
	std::list\
<listen_unit> _listen;
	std::list<std::string> _methods;
	int _error;
	std::string _cgi_loc;
	std::string _def_file;
	std::string _storage;
	size_t _max_client_body;
	std::map<int,
			std::string> _err_location;
	int _autoindex;
	int _workers;

	bool _pathComp(char const *, char const *);

	std::map<std::string, location_unit>::iterator _getLocation(std::string
	                                                            const &);

	std::string _python_path;
	std::string _php_path;

public:
	std::list<std::string> &setName(void);

	std::map<std::string, \
                            location_unit> &setLocation(void);

	std::list<listen_unit> &setListen(void);

	std::list<std::string> &setMethods(void);

	int &setError(void);

	std::string &setCGI_loc(void);

	std::string &setStorage_loc(void);

	size_t &setMax_client_body(void);

	std::map<int, std::string> &setErr_location(void);

	int &setAutoindex(void);

	config_unit &operator=(config_unit const &);

	void setWorkers(int);

	void resort(void);

	void setDefaultFile(std::string const &);

	void setFileStorage(std::string const &);

	config_unit(void);

	~config_unit(void);

	std::list<std::string> const &getName(void) const;

	std::map<std::string, \
                            location_unit> const &getLocation(void) const;

	std::list<listen_unit> const &getListen(void) const;

	std::list<std::string> const &getMethods(void) const;

	int getError(void) const;

	std::string const &getCGI_loc(void) const;

	size_t getMax_client_body(std::string const &);

	std::map<int, std::string> const &getErr_location(void) const;

	int getAutoindex(void) const;

	int getWorkers(void) const;

	std::string const searchError_page(int);

	bool checkMethod(std::string\
 const &method, std::string const &path);

	std::string const &getDefaultFile(void);

	std::string getServerPath(std::string const &);

	std::string getPathFromLocation(std::string const &);

	int checkAutoindex(std::string const &);

	bool checkRedirect(std::string const &);

	std::pair<int, std::string> getRedirectPath(std::string\
 const &);

	std::string getCGI_Path(std::string const &);

	std::string getUploadPath(std::string const &);

	void setPythonExec(std::string const &str);

	void setPHPexec(std::string const &str);

	std::string const &getPythonExec(void);

	std::string const &getPHPexec(void);

};

#endif
