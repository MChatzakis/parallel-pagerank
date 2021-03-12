#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "graph.h"

#define LINE_SIZE 100
#define THREAD_NUM 10
#define D_FACTOR 0.85

typedef struct thread_data
{
    node_t *from;
    node_t *to;
} thread_data;

pthread_t threads[THREAD_NUM];
thread_data data[THREAD_NUM];

void read_csv(char *filename, graph_t *g);
void write_csv(char *filename, graph_t *g);

void *calculate_pagerank(void *arg);

int main(int argc, char **argv)
{
    graph_t *g;
    node_t *from, *to, *iter;
    char *filename = "datasets/enron.txt";
    double d_factor = 0.85, init_pr = 1.0;
    int border = 0, i, counter = 0;

    g = graph_initialize(init_pr);

    /*iter = g->head;
    for (i = 0; i < THREAD_NUM; i++)
    {
        border = (g->size) / THREAD_NUM;
        from = iter;
        while (iter != NULL && counter < border)
        {
            iter = iter->next;
        }
        to = iter;
        //find borders ...
        data[i].from = from;
        data[i].to = to;
        pthread_create(&threads[i], NULL, calculate_pagerank, (void *)data[i]);
    }

    for (i = 0; i < THREAD_NUM; i++)
    {
        pthread_join(threads[i], NULL);
    }
    */
    read_csv(filename, g);

    //graph_print_nodes(g);
    //write_csv("pagerank.csv", g);

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

        //printf("From %ld", from);
        //printf("From %s ", tok);

        tok = strtok(NULL, "\r \t\v");
        to = atol(tok);

        //printf(" to %ld\n", to);
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