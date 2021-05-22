#include "Config_file_parser.hpp"
#include <iostream>

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

void printLocations(std::map<std::string,std::string> const &location)
{
	auto it = location.begin();
	auto end = location.end();

	std::cout << "Locations: \n";
	while (it != end)
		std::cout << "   req " << it->first << "\n    loc " << (it++)->second << "\n\n";
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
	printNames(pars.name);
	printListen(pars.listen);
	printMethods(pars.methods);
	std::cout << "Error: " << pars.error << "\n";
	std::cout << "cgi_location: " << pars.cgi_loc << "\n";
	printLocations(pars.location);
	std::cout << "Max_cliend_body: " << pars.max_client_body << "\n";
	std::cout << "Autoindex: " << pars.autoindex << "\n";
	printerrloc(pars.err_location);
	std::cout << "================\n";
}

int main(int argc, char **argv)
{
	Config_parser pars;
	config_unit conf;
	std::string file;
	if (argc != 2)
		file = "test.conf";
	else
		file = argv[1];
	if (!pars.init(file.c_str()))
		return (0);
	auto it = pars.getConf().begin();
	auto end = pars.getConf().end();
	while (it != end)
		printIt(*it++);
	return (0);
}
