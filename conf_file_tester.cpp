#include "Config_parser.hpp"
#include <iostream>
#include <fstream>

void printNames(std::list<std::string> const &names)
{
	auto st = names.begin();
	auto end = names.end();
	std::cout << "Names: \n";
	while (st != end)
		std::cout << "    " << *st++ << "\n";
}

void printListen(std::list<listen_unit> const &listen)
{
	auto it = listen.begin();
	auto end = listen.end();
	std::cout << "Listen: \n";
	while (it != end)
	{
		std::cout << "    " << it->str << ":" << it->port << " (";
		for (int i = 0; i < 4; i++)
			std::cout << (int)it->digit[i] << ".";
		std::cout << ") " << it->type << "\n";
		++it;
	}
}

void printMethods(std::list<std::string> const &methods)
{
	auto it = methods.begin();
	auto end = methods.end();

	std::cout << "Accepted methods: \n";
	while (it != end)
		std::cout << "    " << *it++ << "\n";
}

void printLocations(std::map<std::string, location_unit> const &location)
{
	auto it = location.begin();
	auto end = location.end();

	std::cout << "Locations: \n---------------\n";
	while (it != end)
	{
		std::cout << "Req " << it->first << "\n"; 
		std::cout << "Abs_path " << it->second._abs_path << "\n";
		std::cout << "Autoindex " << it->second._autoindex << "\n";
		std::cout << "Default file " << it->second._def_file << "\n";
		printMethods(it->second._methods);
		if (!it->second._redirect.second.empty())
		{
			std::cout << "Redirection to " << it->second._redirect.second <<
			" with " << it->second._redirect.first << " code\n";
		}
		++it;
		std::cout << "---------------\n";
	}
	
}

void printerrloc(std::map<int, std::string> const&err_location)
{
	auto it = err_location.begin();
	auto end = err_location.end();

	std::cout << "Err_location: \n";
	while (it != end)
		std::cout << it->first << "     " << (it++)->second << "\n";
}

void printIt(config_unit const &pars)
{
	printNames(pars.getName());
	printListen(pars.getListen());
	printMethods(pars.getMethods());
	std::cout << "Error: " << pars.getError() << "\n";
	std::cout << "cgi_location: " << pars.getCGI_loc() << "\n";
	printLocations(pars.getLocation());
	std::cout << "Max_cliend_body: " << pars.getMax_client_body() << "\n";
	std::cout << "Autoindex: " << pars.getAutoindex() << "\nWorkers: " << pars.getWorkers() << "\n";
	printerrloc(pars.getErr_location());
	std::cout << "================\n";
}

/////////////////////////////////////////////////
#include <dirent.h>
#define FILE_INFO struct dirent
/*
autoindex(string1, string2)
string1 - абсолютный путь к папке
string2 - путь, запрашиваемый в запросе (нужен для 
составления строки с http:// и так далее)
*/
std::string	autoindex(std::string path, std::string request)
{
	DIR						*folder;
	FILE_INFO				*file;
	int 					cnt;
	std::string				page_folders;
	std::string 			page_files;

	if ((cnt = request.rfind("index.")) != std::string::npos)
		request.resize(cnt);
	if (request.back() != '/')
		request.append("/");
	request.insert(0, "http://");
	folder = opendir(path.c_str());
	if (folder == NULL) 
		return (page_folders);
	file = readdir(folder);
	page_folders = "<body>";
	while (file != NULL)
	{
			if (file->d_type == 4)
			{
	//			page_folders.append("[FOLDER] ");
				page_folders.append("<a href = \"");
				page_folders.append(request);
				page_folders.append(file->d_name);
				if (file->d_type == 4)
					page_folders.append("/");
				page_folders.append("\">[");
				page_folders.append(file->d_name);
				page_folders.append("]</a><br>");
			}
			else
			{
				page_files.append("<a href = \"");
				page_files.append(request);
				page_files.append(file->d_name);
				if (file->d_type == 4)
					page_files.append("/");
				page_files.append("\">");
				page_files.append(file->d_name);
				page_files.append("</a><br>");
			}
		file = readdir(folder);
	}
	closedir(folder);
	page_folders.append(page_files);
	page_folders.append("</body>");
	return page_folders;
}
/////////////////////////////////////////////////
int main(int argc, char **argv)
{
	Config_parser pars;
	config_unit conf;
	std::string file;
	std::ofstream out("test.html");

	if (!pars.init("test2.conf"))
		return (0);
/*	auto it = pars.getConf().begin();
	auto end = pars.getConf().end();
	while (it != end)
		printIt(*it++);
	out << autoindex("/home/ksilver/ft_www/site/in_site", "ft_serv/in_site");
	*/if (argc != 2)
		return (0);
	file = argv[1];
	config_unit *loc;
	
	loc = pars.getServerConf(file, 80);
	std::cout << "\n=========\nFinded\n";
	printIt(*loc);
	std::cout << loc->getServerPath("/phpff/yuuy/qwe.html") << "\n";
	std::cout << loc->getServerPath("/img/") << "\n";
	std::cout << loc->checkAutoindex("/phpff/") << "\n";
	std::cout << loc->searchError_page(503) << "\n";
	std::cout << loc->searchError_page(404) << "\n";
	std::cout << loc->checkMethod("PUT", "/img/dragon.jpg") << "\n";
	std::cout << loc->checkMethod("PUT", "/phpff/fast.php") << "\n";
	std::cout << loc->getRedirectPath("/img/cat.jpg").second << "\n";
	std::cout << loc->checkRedirect("/phpff/cat.jpg") << "\n";
	std::cout << loc->getCGI_Path("php/hiho.mp3") << "\n";
	return (0);
}
