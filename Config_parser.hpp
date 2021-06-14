#ifndef CONFIG_FILE_PARSER_H
#define CONFIG_FILE_PARSER_H

#include <string>
#include <cstring>
#include <list>
#include <unistd.h>
#include <fcntl.h>
#include <map>
#include <vector>
#include <sys/stat.h>
#include "Config_unit.hpp"
#include <cstdlib>

#include <iostream> //to do delete

//Hardcode defines
#define CFP_MAIN_FOLDER "./www/"
#define CFP_LOCALHOST "127.0.0.1"

//Errors defines
#define CANT_OPEN 1
#define WRONG_BRCKT 2
#define BAD_LISTEN 3
#define BAD_LOCATION 4
#define ALREDY_NAMED 5
#define BAD_CGI_LOC 6
#define CL_BODY_SZ 7
#define ERR_PAGES 8
#define UNKNWN_TTL 9
#define AUTOINDEX 10
#define OUT_OF_SER 11
#define BAD_WORKERS 12
#define BAD_STORAGE 13

class Config_parser {
	std::map
			<int, \
      std::list\
<config_unit *> > _ports;
	std::vector<char> _file;
	std::list
			<config_unit> _conf;
	config_unit *_act;
	location_unit *_a_loc;
	int _breck;
	std::string _main_folder;
	int _error;

	bool _open_file(char const *);

	void _read_file(int, int);

	bool _recheck_breckts(char const *);

	void _name_filling(char const *);

	char const *_context(int);

	char const *_context(char const *);

	bool _normal(char const *, char const *);

	void _breckets(int);

	bool _semicolon(int);

	int _step_back(int);

	void _map_filling(void);

	bool _check_parsed_data(void);

	bool _check_location(config_unit &);

	bool _check_file(std::string const &);

	bool _check_dir(std::string const &);

	bool _check_path(std::string const &);

	void _check_methods();

	void _check_storage(config_unit &, location_unit &);

	bool _check_cgi_loc(config_unit &);

	bool _check_err_loc(config_unit &);

	bool _check_doubling_server(void);

	bool _check_doubling_server_two(\
                                        std::list<config_unit>::iterator);

	bool _check_doubling_server_name\
(std::list<std::string> &, \
                                        std::list<std::string> &);

	bool _check_doubling_server_listen\
(std::list<listen_unit> const &, \
                                        std::list<listen_unit> const &);

	void _pars_workers(char const *);

	void _pars_location(char const *);

	void _pars_cgi_loc(char const *);

	void _pars_listen(char const *);

	void _client_body_size(char const *);

	void _pars_error_pages(char const *);

	void _methods_filling(char const *);

	void _autoindex(char const *);

	void _methods_filling_loc(char const *);

	void _pars_storage(char const *);

	void _pars_def_file(char const *);

	void _pars_loc_path(char const *);

	void _pars_redirection(char const *);

	void _pars_python_exec(char const *);

	void _pars_php_exec(char const *);

	bool _check_execs(config_unit &);

public:
	Config_parser();

	Config_parser(Config_parser &);

	~Config_parser();

	Config_parser &operator=(Config_parser const &);

	bool init(char const *);

	std::list<config_unit> &getConf(void);

	std::map<int, std::list<config_unit *> > const &getPortsMap(void);

	std::vector<std::pair<std::string, int> > getAllListen(void);

	config_unit *getServerConf(std::string host, int port);
};

#endif

/*
Папка по умолчанию [/tmp/ft_www/] потому что в корне лучше не надо

Необходимо любой сервер описывать внутри server {},
	даже если он один в файле

После "}" не ставятся ";"

"." в начале {[local_path]} в директивах location, cgi_location и
error_pages будет заменена на путь из обязательной директивы 
"location {[path]}"

1) 
	listen [adrs]:[port];
	а) содержит одну запись за раз
	б) не уникальна для сервера
	в) заканчивается ";"
	г) может содержать только adrs или только port
	д) [adrs] может быть буквенным или IPv4
	если [adrs] состоит из 4 блоков чисел 0-255 разделенных точками,
	то он считается IPv4, если только число, без каких либо других знаков
	то он считается портом, все остальные случаях - строка
	е) любой [ards] без сегмента [port] получит порт 80
	ё) в случае отсутствия такой директивы будет сервер будет слушать пор 80
		ё1) запись "...:0" станет портом 80
		ё2) запист ":80" выдаст ошибку
2)
	location /[request_part] {[local_part]}
	a) содержит одну запись за раз
	б) требуется миннимум одна уникальная запись по шаблону
		location {/path/to/serv/dir}
		без [request_part], которая будет хранить путь по умолчанию
	в) не содержит на конце ";"
	г) [local_part] обязательная всегда
		[request_part] обязательная для всех кроме пункта Б
	д) полный путь на серверной машине /tmp/ft_www/[local_part]
		
3) 
	error_pages [err_code1] [err_code2] [err_...] {[loc]/^[file_name]}
	error_pages [err_code1] [err_code2] [err_...] {[loc]/[file_name]}
	error_pages [err_code1] [err_code2] [err_...] {[loc]/[file_name]^.ext}
	a) содержит множество err_code1, состоящих из цифр, разделённых пробелами	
	б) если внутри {} присутствует ^ то ^ будет заменём на каждое err_code
		б1) заменяется только самая правая ^, остальные не трогаются
		б2) возможна запись 
		error_pages 404 {./^/err.html}
		такая запись откроет фаил по адресу /tmp/ft_www/site_dir/404/err.html
			
4)	
	client_max_body_size [size];
	client_max_body_size [size]b;
	a) size является unsigned int'ом, указывает максимальный размер чтения тела
		присланного запроса в КИЛОБАЙТАХ (число будет умноженно на 1024)
	б) при написании "b" будет указан размер в байтах (без умножения)
	в) при отсутствии директивы либо же 0 будет установлен размер -1
		(т.е. максимальный для unsigned int)
	г) в зачёт идёт последняя запись client_max_body_size
	
5) 
	accepted_methods GET POST HEAD OPTIONS;
	a) принимает только эти четыре метода (зашлавными буквами)
		а1) или меньше из этих четырёх 
	б) при пустой или отсутвствующей дериктиве буду включены все
	методы
	в) приналичии нескольких директив последующие будут добавлены к имеющимся
	
6)
	server_name [str] [str] ...;
	а) принимает строки разденённые пробелами
	б) создаёт список имён сервера
	в) приналичии нескольких директив последующие будут добавлены к имеющимся
	
7)
	cgi_location {[local_path]}
	а) считает [path] путём к директории
	б) в зачёт идёт последняя запись
	в) [local_path] начинающийcя с "." прибавит к себе в начало путь из 
	location {}
	в1) [local_path] начинающийся не с точки прибавит к себе в начало 
	/tmp/ft_www/  
	г) пустая или отсутсвующая директива скорирует путь из location {}
	
8)
	autoindex on/off;
	a) только on или off, пустой считается ошибкой 
	б) по умолчанию off
	
9) 
	worker_processes [unsigned int];
	а) количество будущих воркеров

10)
	default_file [string];
	
11)
	redirect 301 https://ya.ru;
	redirect 301 https://ya.ru^;
	
	только для location
	при наличии ^ добавит путь к редиректу 
	то есть: 
	location /img/ {
		redirect 301 http://google.com^;
		}
	превратит
	www.your_site.ru/img/kitty.jpg
	в 
	http://google.com/kitty.jpg
	
	при отсутствии ^ 
	location /img/ {
		redirect 301 http://google.com;
		}
	вернёт просто http://google.com
	
	
	max_body_size [int];
*/
