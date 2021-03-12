#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "graph.h"

#define LINE_SIZE 100
#define THREADS_NUM 2
#define D_FACTOR 0.85
#define ITERATIONS 50
#define DEBUG 0

typedef struct thread_data
{
    long from;
    long to;
} thread_data;

pthread_mutex_t mutex;
pthread_attr_t attr;

pthread_t threads[THREADS_NUM];
thread_data data[THREADS_NUM];

graph_t *read_file(char *filename);
void *pagerank_calculate(void *arg);
void write_csv(char *filename, graph_t *g);

int main()
{
    graph_t *g;
    long i, sets = 0, from = 0, to = 0;

    g = read_file("datasets/enron.txt");
    sets = g->size / THREADS_NUM;

    pthread_mutex_init(&mutex, NULL);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (i = 0; i < THREADS_NUM; i++)
    {
        from = i * sets;
        to = (i + 1) * sets;
        data[i].from = from;
        data[i].to = to;
    }

    if ((sets = g->size % THREADS_NUM) > 0)
    {
        data[THREADS_NUM - 1].to = g->size;
    }

    for (i = 0; i < THREADS_NUM; i++)
    {   
        //printf("Thread %ld takes %ld to %ld.\n", i, data[i].from, data[i].to);
        pthread_create(&threads[i], NULL, pagerank_calculate, &data[i]);
    }

    for (i = 0; i < THREADS_NUM; i++)
    {
        pthread_join(threads[i], NULL);
    }

    //graph_print(g, stdout);
    write_csv("pagerank.csv", g);
    return 0;
}

void write_csv(char *filename, graph_t *g)
{
    FILE *stream;
    if (!(stream = fopen(filename, "w+")))
    {
        perror("Could not open the file");
        exit(EXIT_FAILURE);
    }

    graph_csv(g, stream);

    fclose(stream);
}

graph_t *read_file(char *filename)
{
    FILE *stream;
    graph_t *g;
    char line[LINE_SIZE], *tok;
    long from, to, size = 0;
    int counter = 0;

    if (!(stream = fopen(filename, "r+")))
    {
        perror("Could not open the file");
        exit(EXIT_FAILURE);
    }

    while ((fgets(line, LINE_SIZE, stream)) != NULL)
    {
        if (line[0] == '#')
        {
            continue;
        }

        tok = strtok(line, "\r \t\v");
        from = atol(tok);

        if (size < from)
        {
            size = from;
        }

        tok = strtok(NULL, "\r \t\v");
        to = atol(tok);
        if (size < to)
        {
            size = to;
        }
    }
    size++;
    printf("Nodes: %ld\n", size);

    g = graph_initialize(1.0, size);

    rewind(stream);

    while ((fgets(line, LINE_SIZE, stream)) != NULL)
    {
        if (line[0] == '#')
        {
            continue;
        }

        tok = strtok(line, "\r \t\v");
        from = atol(tok);
        //printf("From %ld to ", from);

        tok = strtok(NULL, "\r \t\v");
        to = atol(tok);
        //printf("%ld\n", to);
        graph_add_link(g, from, to);
    }

    fclose(stream);
    return g;
}

void *pagerank_calculate(void *arg)
{
    thread_data *data = (thread_data *)arg;

    printf("Managing from: %ld to %ld.\n", data->from, data->to);

    return NULL;
}