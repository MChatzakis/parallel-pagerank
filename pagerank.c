#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "graph.h"

#define LINE_SIZE 100

void read_csv(char *filename, graph_t *g);
void write_csv(char *filename, graph_t *g);

int main(int argc, char **argv)
{
    char *filename = "datasets/gnutella.txt";
    graph_t *g;
    node_t *t;

    g = graph_initialize(1.0);

    read_csv(filename, g);

    graph_print_nodes(g);

    write_csv("pagerank.csv", g);

    return 0;
}

void read_csv(char *filename, graph_t *g)
{
    FILE *stream;
    char line[LINE_SIZE], *tok;
    long from, to;
    int counter = 0;

    if (!(stream = fopen(filename, "r+")))
    {
        perror("Could not open the file");
        exit(EXIT_FAILURE);
    }

    while ((fgets(line, LINE_SIZE, stream)) != NULL) // && counter < 20)
    {
        if (line[0] == '#')
        {
            continue;
        }

        tok = strtok(line, "\r \t\v");
        from = atol(tok);

        printf("From %ld", from);
        //printf("From %s ", tok);

        tok = strtok(NULL, "\r \t\v");
        to = atol(tok);

        printf(" to %ld\n", to);
        //printf("To %s\n", tok);

        graph_add_link(g, from, to);
        counter++;
    }

    fclose(stream);
}

void write_csv(char *filename, graph_t *g)
{
    FILE *stream;

    if (!(stream = fopen(filename, "w+")))
    {
        perror("Could not open the file");
        exit(EXIT_FAILURE);
    }

    graph_output(g, stream);

    fclose(stream);
}