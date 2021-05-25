#ifndef CONFIG_UNIT_H
#define CONFIG_UNIT_H

#include <string>
#include <map>
#include <list>

struct listen_unit
{
	std::string 	str;
	unsigned char 	digit[4];
	char 			type;
	int				port;
	
	listen_unit &	operator=(listen_unit const &);
};

class config_unit
{
	
	std::list<std::string>	_name;
	std::map<std::string,\
			std::string>	_location;
	std::list\
		<listen_unit>		_listen;
	std::list<std::string>	_methods;
	int						_error;
	std::string				_cgi_loc;
	unsigned int			_max_client_body;
	std::map<int, 
		std::string> 		_err_location;
	bool 					_autoindex;
	int 					_workers;
	public:
		std::list<std::string> 				& setName(void);
		std::map<std::string, std::string> 	& setLocation(void);
		std::list <listen_unit>				& setListen(void);
		std::list<std::string>				& setMethods(void);
		int									& setError(void);
		std::string							& setCGI_loc(void);
		unsigned int						& setMax_client_body(void);
		std::map<int,  std::string> 		& setErr_location(void);
		bool 								& setAutoindex(void);
		config_unit 						& operator=(config_unit const &);
		void 								  setWorkers(int);
		
		config_unit(void);
		~config_unit(void);
		
		std::list<std::string> 				const& getName(void) const;
		std::map<std::string, std::string>	const& getLocation(void) const;
		std::list <listen_unit>				const& getListen(void) const;
		std::list<std::string>				const& getMethods(void) const;
		int										   getError(void) const;
		std::string							const& getCGI_loc(void) const;
		unsigned int							   getMax_client_body(void)\
																		 const;
		std::map<int,  std::string> 		const& getErr_location(void) const;
		bool 									   getAutoindex(void) const;
		int                                 	   getWorkers(void) const;
		
		std::string 						const& searchError_page(int);
		bool 								 	   checkMethod(std::string\
																	const &);
		
}; 

#endif