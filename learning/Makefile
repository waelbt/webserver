SERVER	= 	server

CLIENT =	client

C_SRCS	=	time_console.cpp

S_SRCS	=	time_server.cpp

C_OBJCT	=	${C_SRCS:.cpp=.o}

S_OBJCT	=	${S_SRCS:.cpp=.o}

CPPFLAGS = -Wall -Wextra -Werror -std=c++98 

CXX = c++

RM		= rm -rf

all	: $(SERVER) $(CLIENT)

$(SERVER) : $(S_OBJCT) $(PRINTF)
	@${CXX} $(CPPFLAGS) $(S_OBJCT) -o $(SERVER)
	@${RM} ${S_OBJCT}

$(CLIENT) : $(C_OBJCT) $(PRINTF)
	@${CXX} $(CPPFLAGS) $(C_OBJCT) -o $(CLIENT)
	@${RM} ${C_OBJCT}

clean   :
	@${RM} ${C_OBJCT}
	@${RM} ${S_OBJCT}

fclean	: clean
	@${RM} ${SERVER}
	@${RM} ${CLIENT}

re		: fclean all

.PHONY	:	all fclean re clean
