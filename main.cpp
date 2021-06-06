
#include "Cluster.hpp"
#include <iostream>

int main(int argv, char **argc)
{
	Cluster *cluster = new Cluster();
	std::string configFile = DEFAULT_CONFIG_PATH;

	if (argv > 2) {
		std::cerr << "Invalid count of arguments!" << std::endl;
		return (1);
	} else if (argv == 2) {
		configFile = argc[1];
	}

	if (!cluster->setup(configFile)) {
		return 1;
	}
	cluster->run();

	return 0;
}
