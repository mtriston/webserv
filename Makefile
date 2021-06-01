COM_COLOR   = \033[0;34m
OBJ_COLOR   = \033[0;36m
OK_COLOR    = \033[0;32m
WARN_COLOR  = \033[0;33m
NO_COLOR    = \033[m

OK_STRING    = [OK]
COM_STRING   = Compiling

NAME = webserv

OBJ_DIR = bin/
HDR_DIR = ./

INCLUDES = -I$(HDR_DIR)

CC = clang++
FLAGS = -Wall -Wextra -Werror -std=c++98 -g -pthread

SRC_FILES = main.cpp\
			Cluster.cpp\
			Response.cpp\
			Request.cpp\
			ASocket.cpp\
			ConnectionSocket.cpp\
			ListenSocket.cpp\
			SocketList.cpp\
			Config.cpp\
			ConfigParser.cpp\
			Worker.cpp\
			IWork.cpp\
			WorkList.cpp\
			WorkerManager.cpp\
			utils.cpp\

HEADER_FILES = Cluster.hpp\
			Response.hpp\
			Request.hpp\
			ASocket.hpp\
			ConnectionSocket.hpp\
			ListenSocket.hpp\
			ISocketList.hpp\
			SocketList.hpp\
			Config.hpp\
			ConfigParser.hpp\
			Worker.hpp\
			IWork.hpp\
			IWorkList.hpp\
			WorkList.hpp\
			WorkerManager.hpp\
			utils.hpp\

OBJ_FILES = $(addprefix $(OBJ_DIR), $(SRC_FILES:.cpp=.o))

all: $(OBJ_DIR) $(NAME) $(LIBFT)

$(OBJ_DIR):
	@mkdir $(OBJ_DIR)

$(NAME): $(OBJ_FILES) $(HEADER_FILES)
	@$(CC) $(FLAGS) $(OBJ_FILES) -o $(NAME)
	@echo "$(OK_COLOR) $(OK_STRING) $(OBJ_COLOR) $(NAME) $(NO_COLOR)"

$(OBJ_FILES): $(OBJ_DIR)%.o : %.cpp $(HEADER_FILES)
	@$(CC) $(FLAGS) -c $< -o $@
	@echo "$(COM_COLOR) $(COM_STRING) $(OBJ_COLOR) $(@) $(NO_COLOR)"

clean:
	@rm -f $(OBJ_FILES)
	@echo "$(WARN_COLOR) All object files have been removed $(NO_COLOR)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(WARN_COLOR) Executable '$(NAME)' has been removed $(NO_COLOR)"

re: fclean all

.PHONY: all clean fclean re
