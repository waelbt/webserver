NAME	= webserve

SRCS		= main.cpp  srcs/ParcingTools.cpp srcs/server.cpp srcs/routes.cpp srcs/Configuration.cpp srcs/ShowData.cpp

OBJ		= ${SRCS:.cpp=.o}

CPPFLAGS = -Wall -Wextra -Werror -std=c++98 

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