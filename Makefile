
SRCS	=	nm.c\
			elf_checker.c\
			elf_checker_x32.c\
			elf_checker_x64.c\
			elf_getter.c\
			elf_getter_x32.c\
			elf_getter_x64.c

#---------------------------
CC		=	gcc
NAME	=	my_nm
LIBS    =	
INCLUDE =	-I ./include
CFLAGS	=	-g3 -Wall -Wextra -Werror
#---------------------------
SRCDIR  =	./srcs
OBJDIR  =	./objs
OBJS	=	$(addprefix $(OBJDIR)/,$(SRCS:.c=.o))
#---------------------------

default:
	make all

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(OBJDIR):
	mkdir $(OBJDIR)

$(NAME): $(OBJDIR) $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) $(LIBS) $(INCLUDE) -o $(NAME)

all:	$(NAME)

clean:
	rm -f $(OBJS)
	rm -Rf $(OBJDIR)

fclean:		clean
	rm -f $(NAME)

re:
	make fclean
	make all

.PHONY:	default all clean fclean re