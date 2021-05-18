#include "Server_unit.hpp"

int main(int argc, char **argv, char **env)
{
	Server_unit serv;
//write(1, "m0\n", 3);
	serv.setEnv(env);
//write(1, "m1\n", 3);
	serv.circule();
	return (0);
}