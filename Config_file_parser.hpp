#ifndef CONFIG_FILE_PARSER_H
#define CONFIG_FILE_PARSER_H

#include <string>
#include <cstring>
#include <list>
#include <unistd.h>
#include <fcntl.h>
#include <map>

#include <iostream> //to do delete

//Errors defines
#define CANT_OPEN 1
#define WRONG_BRCKT 2
#define BAD_LISTEN 3
#define BAD_LOCATION 4
#define ALREDY_NAMED 5
#define BAD_CGI_LOC 6
#define CL_BODY_SZ 7
#define ERR_PAGES 8

/*
struct name_unit
{
	std::string 	str;
	unsigned char 	digit[4];
	char 			type;
	
	name_unit &		operator=(name_unit const &);
};
*/
struct listen_unit
{
	std::string 	str;
	unsigned char 	digit[4];
	char 			type;
	int				port;
//	bool 			def_serv;
	
	listen_unit &	operator=(listen_unit const &);
};

struct loc_unit
{
	std::string 	local_path;
	std::string		request_path;

	loc_unit &		operator=(loc_unit const &);
};

struct config_unit
{
	std::list<std::string>	name;
	std::list<loc_unit>		location;
	std::list\
		<listen_unit>		listen;
	int						port;
	int						error;
	std::string				cgi_location;
	unsigned int			max_client_body;
//	std::string 			err_location;
	std::map<int, 
		std::string> 		err_location;
	
	config_unit &			operator=(config_unit const &);	
};

class Config_parser
{
	std::string			_file;//to do перепиши в вектор, всё равно не пользуешься
	std::list
	  <config_unit>		_conf;
	
	config_unit *		_act;
	int					_breck;

	bool 			_open_file(char const *);
	void 			_read_file(int, int);
	bool 			_recheck_breckts(char const *);
	void			_name_filling(char const *);
	char const *	_context(int);
	char const *	_context(char const *);
	bool			_normal(char const*, char const *);
	void 			_breckets(int);
	void			_semicolon(int);
	int				_step_back(int);
	void			_map_filling(void);
	
	void 			_pars_location(char const *);
	void 			_pars_cgi_location(char const *, char);
	void			_pars_listen(char const *);
	void 			_client_body_size(char const *);
	void			_pars_error_pages(char const *);
	void			_pars_error_pages_two(char const *);
	public:
		Config_parser();
		Config_parser(Config_parser&);
		~Config_parser();
		Config_parser &operator=(Config_parser const &);
		void init(char const*);
		
		std::list<config_unit> &getConf(void);
		
		std::map
	  <int,\
	  std::list\
	  <config_unit*> >	_ports;//to do убери в приват
};

#endif



/*
1) Необходимо любой сервер описывать внутри server {},
даже если он один в файле
2) После "}" не ставятся ";"
3) 
	listen [adrs]:[port];
	а) содержит одну запись за раз
	б) не уникальна для сервера
	в) заканчивается ";"
	г) может содержать только adrs или только port
	д) adrs может быть буквенным или IPv4
	если adrs состоит из 4 блоков чисел 0-255 разделенных точками,
	то он считается IPv4, если только число, без каких либо других знаков
	то он считается портом, все остальные случаи - ards
	е) любой ards без сегмента [port] получит порт 80
	ё) в случае отсутствия такой директивы будет сервер будет слушать пор 80
	
4)
	location request_part {local_part}
	a) содержит одну запись за раз
	б) требуется миннимум одна уникальная запись по шаблону
		location {/path/to/serv/dir}
		без request_part, которая будет хранить путь по умолчанию
	в) не содержит на конце ";"
	г) local_part обязательная всегда
		request_part обязательная для всех кроме пункта Б
		
5) 
	error_pages [err_code1] [err_code2] [err_...] {[loc]/^[file_name]}
	error_pages [err_code1] [err_code2] [err_...] {[loc]/[file_name]}
	error_pages [err_code1] [err_code2] [err_...] {[loc]/[file_name]^.ext}
	a) содержит множество err_code1, состоящих из цифр, разделённых пробелами
	б) при отсутствии "/" [loc] будет заменён на значение взятое из 
	"location {}"	
	в) если внутри {} присутствует ^ то ^ будет заменём на каждое err_code
		б1) ^ только самая правая, остальные не трогаются
		б2) возможна запись 404 {./^/err.html}, такая запись откроет  
			по адресу /site_dir/404/err.html
*/