NAME	= webserve

SRC		= main.cpp  srcs/ParcingTools.cpp srcs/server.cpp srcs/routes.cpp srcs/Configuration.cpp srcs/ShowData.cpp

OBJ		= ${SRC:.cpp=.o}

CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g

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