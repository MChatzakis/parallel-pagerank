CC = gcc

GRAPH = graph/graph.h
GRAPH_IMP = graph/graph.c

PGRANK = pagerank.c

EXEC_FILE = page_rank
GCC_FLAGS = -Werror -pthread -o

all: gcc

run: all
	./$(EXEC_FILE)

gcc: $(PGRANK) $(GRAPH) $(GRAPH_IMP)
	$(CC) $(PGRANK) $(GRAPH_IMP) $(GCC_FLAGS) $(EXEC_FILE)

clean:
	rm -f page_rank