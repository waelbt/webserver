NAME	= webserve

PARSING = utils commonEntity configuration location server client webserver request response post

SRCS	= $(addsuffix .cpp, $(addprefix srcs/, $(PARSING))) \
			main.cpp

OBJ		= ${SRCS:.cpp=.o}

CPPFLAGS = -Wall -Wextra -Werror -std=c++98 #-g -fsanitize=address

CXX = c++

RM		= rm -rf

all	: $(NAME)

${NAME}	: ${OBJ}
	${CXX} $(CPPFLAGS) $(OBJ) -o $(NAME)

clean   :
	${RM} ${OBJ}

fclean	: clean
	${RM} $(NAME)

re		: fclean all

.PHONY	:	all fclean re clean