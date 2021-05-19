COM_COLOR   = \033[0;34m
OBJ_COLOR   = \033[0;36m
OK_COLOR    = \033[0;32m
ERROR_COLOR = \033[0;31m
WARN_COLOR  = \033[0;33m
NO_COLOR    = \033[m

OK_STRING    = [OK]     
ERROR_STRING = [ERROR]
WARN_STRING  = [WARNING]
COM_STRING   = Compiling

NAME = libft.a

SRC_DIR = src/
OBJ_DIR = bin/
HDR_DIR = includes/

INCLUDES = -I$(HDR_DIR)

CC = gcc
FLAGS = -Wall -Werror -Wextra -g

CHAR_DIR = char/
CHAR_FILES = $(addsuffix .c, $(addprefix $(CHAR_DIR),\
			ft_isblank\
            ft_isspace\
			ft_isalpha\
			ft_isdigit\
			ft_isalnum\
			ft_isascii\
			ft_isprint\
			ft_toupper\
			ft_tolower\
			))

GC_DIR = garbage_collector/
GC_FILES = 	$(addsuffix .c, $(addprefix $(GC_DIR),\
			free_gc\
			malloc_gc\
			memory_manager\
			realloc_gc\
			))

LST_DIR = linked_list/
LST_FILES = $(addsuffix .c, $(addprefix $(LST_DIR),\
			ft_lstnew\
			ft_lstadd_front\
			ft_lstsize\
			ft_lstadd_back\
			ft_lstdelone\
			ft_lstclear\
			ft_lstiter\
			ft_lstmap\
			ft_lstlast\
			ft_lst_remove_if\
			))

MEM_DIR = mem/
MEM_FILES = $(addsuffix .c, $(addprefix $(MEM_DIR),\
			ft_memset\
			ft_bzero\
			ft_memcpy\
			ft_memccpy\
			ft_memmove\
			ft_memchr\
			ft_memcmp\
			ft_calloc\
			))

NBR_DIR = nbr/
NBR_FILES = $(addsuffix .c, $(addprefix $(NBR_DIR),\
			ft_atoi\
			ft_atoi_base\
			ft_itoa\
			ft_itoa_base\
			ft_max\
			))

PRINT_DIR = print/
PRINT_FILES =	$(addsuffix .c, $(addprefix $(PRINT_DIR),\
				ft_putchar_fd\
				ft_putstr_fd\
				ft_putendl_fd\
				ft_putnbr_fd\
				))

STR_DIR = str/
STR_FILES =	$(addsuffix .c, $(addprefix $(STR_DIR),\
			ft_strlen\
			ft_strcpy\
			ft_strlcpy\
			ft_strlcat\
			ft_strchr\
			ft_strrchr\
			ft_strnstr\
			ft_strncmp\
			ft_strcmp\
			ft_strdup\
			ft_substr\
			ft_strjoin\
			ft_strtrim\
			ft_split\
			ft_strmapi\
			ft_free_array\
			))

GNL_DIR = get_next_line/
GNL_FILES = $(addsuffix .c, $(addprefix $(GNL_DIR),\
			get_next_line\
			))

SRC_FILES = $(CHAR_FILES) $(GC_FILES) $(LST_FILES) $(MEM_FILES) $(NBR_FILES) $(PRINT_FILES) $(STR_FILES) $(GNL_FILES)

OBJ_FILES = $(addprefix $(OBJ_DIR), $(SRC_FILES:.c=.o))

HEADER = libft.h

all: $(OBJ_DIR) $(NAME)

$(NAME): $(OBJ_FILES) $(HEADER)
	@ar rc $(NAME) $(OBJ_FILES)
	@ranlib $(NAME)
	@echo "$(OK_COLOR) $(OK_STRING) $(OBJ_COLOR) $(NAME) $(NO_COLOR)"

$(OBJ_FILES): $(OBJ_DIR)%.o : $(SRC_DIR)%.c $(HEADER)
	@$(CC) $(FLAGS) -c $< -o $@
	@echo "$(COM_COLOR) $(COM_STRING) $(OBJ_COLOR) $(@) $(NO_COLOR)"

$(OBJ_DIR):
	@mkdir $(OBJ_DIR)
	@mkdir $(addprefix $(OBJ_DIR), $(CHAR_DIR) $(GC_DIR) $(LST_DIR) $(MEM_DIR) $(NBR_DIR) $(PRINT_DIR) $(STR_DIR) $(GNL_DIR))

clean:
	@rm -rf $(OBJ_DIR)
	@echo "$(WARN_COLOR) All object files have been removed $(NO_COLOR)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(WARN_COLOR) $(NAME) has been removed $(NO_COLOR)"

re: fclean all