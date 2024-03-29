CXX = c++
# -Wall -Wextra -Werror -pedantic-errors
CXXFLAGS = -std=c++17 -MMD -MP -g
CPPFLAGS = 
LDFLAGS = 
LDLIBS = -lglfw -lGLEW -framework OpenGL
NAME = scop
INCDIR	=	$(shell brew --prefix glew)/include $(shell brew --prefix glfw)/include includes
INCS	=	$(addprefix -I,$(INCDIR))

# ########### SRCS ########### #
SRCS = main.cpp ft_glm.cpp controls.cpp load_obj.cpp load_bmp.cpp load_shader.cpp load_material.cpp
VPATH = srcs
# ############################ #

# ########### OBJS ########### #
OBJSDIR = obj
OBJS = $(addprefix $(OBJSDIR)/, $(SRCS:%.cpp=%.o))
DEPENDS = $(OBJS:.o=.d)

# ############################ #

all: $(OBJSDIR) $(NAME)

$(OBJSDIR):
	mkdir -p $(OBJSDIR)

$(NAME): $(OBJS)
	$(CXX) -o $(NAME) $(OBJS) $(LDFLAGS) $(LDLIBS)

$(OBJSDIR)/%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(INCS) -o $@ -c $<

clean:
	$(RM) $(OBJS) $(DEPENDS)

fclean: clean
	$(RM) $(NAME)

format:
	clang-format -style=file -i $(VPATH)/* $(INCDIR)/*

# ############################### #

re: fclean all

-include $(DEPENDS)
.PHONY: all clean fclean re