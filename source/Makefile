CC = gcc
CC_FLAGS = -Werror -pthread -o

MAIN = pagerank.c
LIB = graph/graph.h
LIB_IM = graph/graph.c

NAME = page_rank

all: gcc

gcc: $(LIB) $(LIB_IM) $(MAIN)
	$(CC) $(MAIN) $(LIB_IM) $(CC_FLAGS) $(NAME)

run: all
	./$(NAME)

clean: 
	rm -rf $(NAME)	