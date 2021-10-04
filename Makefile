NAME	= ft_vox

SRCS 	= ./srcs/main.cpp ./srcs/chunk.cpp ./srcs/frustum.cpp ./srcs/simplex.cpp ./srcs/cubemap.cpp 

OBJ_DIR = ./.objs
SRC_DIR = ./srcs
INC_DIR = ./include
INC_HDR =	-I./ -I./glfw/include -I./gl3w/include/GL
INC_LIB =	-L./glfw/src -lglfw3 -lpthread -pthread\
			-framework OpenGL -framework AppKit -framework Cocoa -framework IOKit -framework CoreVideo
INC_H	= ./headers/vox.hpp ./headers/chunk.hpp./headers/const.hpp ./headers/cube.hpp ./headers/frustum.hpp ./headers/mesh.hpp ./headers/shader.hpp ./headers/simplex.hpp ./headers/stb_image.hpp ./headers/vector3.hpp

CFLAGS	:= -std=c++11 -stdlib=libc++ -Wall -Wextra -O3 -I$(INC_DIR)
OBJS	:= $(patsubst .$(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,  $(SRCS))

all: CREATE_OBJDIR $(NAME)

CREATE_OBJDIR:
		@mkdir -p $(OBJ_DIR)

$(NAME): $(OBJS)
		@g++ ${CFLAGS} ${INC_LIB} $^ -o ${NAME}
		printf "\033[1m[ ft_vox ]\033[1m\t\tcompilation complete.\t\033[0m\n"

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp $(INC_H)
		g++ $(CFLAGS) -o $@ -c $<

mk: 
	@mkdir -p .objs

clean:
		@rm -rf $(OBJ_DIR)
		@echo "\033[1m[ ft_vox ]\033[0m\t\tobjects cleared.\033[0m"

fclean : clean
		@rm -f $(NAME)
		@echo "\033[1m[ ft_vox ]\033[0m\t\tbinary deleted.\033[0m"
re:		fclean all