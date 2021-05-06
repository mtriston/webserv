NAME = webserv

SRC = main.cpp Response.cpp Request.cpp Server.cpp ServerCluster.cpp Session.cpp utils.cpp Config.cpp ConfigParser.cpp

HEADER = Request.hpp Response.hpp Server.hpp ServerCluster.hpp Session.hpp utils.hpp Config.hpp ConfigParser.hpp

FLAGS = -Wall -Wextra -Werror

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ) $(HEADER)
	clang++ $(FLAGS) $(OBJ) -o $(NAME)

%.o : %.cpp
	clang++ $(FLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
