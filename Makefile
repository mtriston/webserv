NAME = webserv

SRC = main.cpp Response.cpp Request.cpp ConnectionSocket.cpp Cluster.cpp ListenSocket.cpp utils.cpp Config.cpp ConfigParser.cpp ASocket.cpp Worker.cpp IWork.cpp

HEADER = Response.hpp Request.hpp ConnectionSocket.hpp Cluster.hpp ListenSocket.hpp utils.hpp Config.hpp ConfigParser.hpp ASocket.hpp Worker.hpp IWork.cpp

FLAGS = -Wall -Wextra -Werror -g -pthread

OBJ = $(SRC:.cpp=.o)

LIBFT_DIR = ./libft

LIBFT = $(LIBFT_DIR)/libft.a

all: $(NAME) $(LIBFT)

$(LIBFT):
	make -C $(LIBFT_DIR)

$(NAME): $(OBJ) $(HEADER)
	clang++ $(FLAGS) $(OBJ) -L $(LIBFT) -o $(NAME)

%.o : %.cpp
	clang++ $(FLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
