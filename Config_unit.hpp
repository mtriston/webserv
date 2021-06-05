#ifndef CONFIG_UNIT_H
#define CONFIG_UNIT_H

#include <string>
#include <map>
#include <list>
#include <cstring>
#include <sstream>

#include <iostream> //delete

struct listen_unit
{
	std::string 	str;
	unsigned char 	digit[4];
	char 			type;
	int				port;
	
	listen_unit &	operator=(listen_unit const &);
};

struct location_unit
{
	std::string				_abs_path;
	std::string  			_def_file;
	std::list<std::string>	_methods;
	int 					_autoindex;
	std::pair<int, \
		std::string>		_redirect;
	
	location_unit & operator=(location_unit const &);
};

class config_unit
{
	
	std::list<std::string>	_name;
	std::map<std::string,\
			location_unit>	_location;
	std::list\
		<listen_unit>		_listen;
	std::list<std::string>	_methods;
	int						_error;
	std::string				_cgi_loc;
	std::string 			_def_file; 	
	unsigned int			_max_client_body;
	std::map<int, 
		std::string> 		_err_location;
	int 					_autoindex;
	int 					_workers;
	bool 					_pathComp(char const *, char const *);
	std::map<std::string, location_unit>::iterator _getLocation(std::string 
																const&);
	public:
		std::list<std::string> 				& setName(void);
		std::map<std::string, \
							location_unit> 	& setLocation(void);
		std::list <listen_unit>				& setListen(void);
		std::list<std::string>				& setMethods(void);
		int									& setError(void);
		std::string							& setCGI_loc(void);
		unsigned int						& setMax_client_body(void);
		std::map<int,  std::string> 		& setErr_location(void);
		int 								& setAutoindex(void);
		config_unit 						& operator=(config_unit const &);
		void 								  setWorkers(int);
		void 								resort(void);
		void 								setStorageLoc(std::string const&);
		void 								setDefaultFile(std::string const &);
		
		config_unit(void);
		~config_unit(void);
		
		std::list<std::string> 				const& getName(void) const;
		std::map<std::string, \
							location_unit>	const& getLocation(void) const;
		std::list <listen_unit>				const& getListen(void) const;
		std::list<std::string>				const& getMethods(void) const;
		int										   getError(void) const;
		std::string							const& getCGI_loc(void) const;
		unsigned int							   getMax_client_body(void)\
																		 const;
		std::map<int,  std::string> 		const& getErr_location(void) const;
		int 									   getAutoindex(void) const;
		int                                 	   getWorkers(void) const;

		
		std::string 						const  searchError_page(int);
		bool 								checkMethod(std::string\
									const &method, std::string const &path);
		std::string 						const& getDefaultFile(void);
		std::string 						getServerPath(std::string const &);
		int 								checkAutoindex(std::string const &);
		
		bool 								checkRedirect(std::string const &);
		std::pair<int,std::string>			getRedirectPath(std::string\
																	 const &);
		std::string  						getCGI_Path(std::string const &);
		
		
}; 

#endif
