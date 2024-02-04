CXX = c++
# -Wall -Wextra -Werror -pedantic-errors
CXXFLAGS = -std=c++17 -MMD -MP -g
# -Iinclude/containers -Iinclude/utils
CPPFLAGS = 
LDFLAGS = 
LDLIBS = -lGLEW -lglfw -framework OpenGL
NAME = scop
INCDIR	=	includes
INCS	=	$(addprefix -I,$(INCDIR))

# ########### SRCS ########### #
SRCS = main.cpp type.cpp load.cpp glm.cpp
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

leaks: $(NAME)
	leaks -q -atExit -- ./$(NAME)

format:
	clang-format -style=file -i $(VPATH)/$(SRCS)

# ############################### #

re: fclean all

-include $(DEPENDS)
.PHONY: all clean fclean re leaks