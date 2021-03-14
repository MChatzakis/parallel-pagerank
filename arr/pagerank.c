#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "graph/graph.h"

#define LINE_SIZE 100
#define THREADS_NUM 3
#define D_FACTOR 0.85
#define ITERATIONS 1
#define DEBUG 0

typedef struct thread_data
{
    long from;
    long to;
    long sets;
} thread_data;

pthread_barrier_t bar;
pthread_attr_t attr;

graph_t *g;

pthread_t threads[THREADS_NUM];
pthread_mutex_t mutex[THREADS_NUM];
thread_data data[THREADS_NUM];

graph_t *read_file(char *filename);
void *pagerank_calculate(void *arg);
void write_csv(char *filename, graph_t *g);

int main()
{
    long i, sets = 0, from = 0, to = 0;

    g = read_file("datasets/simple.txt");
    sets = g->size / THREADS_NUM;

    for (i = 0; i < THREADS_NUM; i++)
    {
        pthread_mutex_init(&mutex[i], NULL);
    }


    pthread_barrier_init(&bar, NULL, THREADS_NUM);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (i = 0; i < THREADS_NUM; i++)
    {
        from = i * sets;
        to = (i + 1) * sets;
        data[i].from = from;
        data[i].to = to;
        data[i].sets = sets;
    }

    if ((sets = g->size % THREADS_NUM) > 0)
    {
        data[THREADS_NUM - 1].to = g->size;
    }

    for (i = 0; i < THREADS_NUM; i++)
    {
        //printf("Thread %ld takes %ld to %ld.\n", i, data[i].from, data[i].to);
        pthread_create(&threads[i], &attr, pagerank_calculate, &data[i]);
    }

    for (i = 0; i < THREADS_NUM; i++)
    {
        pthread_join(threads[i], NULL);
    }

    //graph_print(g, stdout);
    write_csv("output/pagerank.csv", g);
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

    printf("File saved: \"%s\"\n", filename);
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
    thread_data *data;
    node_t *nodes;
    link_t *link;
    long i, j, thread_res, sets;
    double score;

    data = (thread_data *)arg;
    //printf("Managing from: %ld to %ld.\n", data->from, data->to);

    assert(g && g->nodes);

    nodes = g->nodes;

    for (int j = 0; j < ITERATIONS; j++)
    {

        pthread_barrier_wait(&bar);

        for (i = data->from; i < data->to; i++)
        {
            if (nodes[i].outlinks_num == 0)
            {
                continue;
            }

            //calc for nodes[i].
            score = (nodes[i].score * D_FACTOR) / nodes[i].outlinks_num; //need sync
            //printf("Node %ld will give %f score to:\n", i, score);

            //assign
            link = nodes[i].outlinks_head;
            while (link != NULL)
            {
                //thread_res = link->to_id / data->sets;
                //printf("Node %ld belongs to %ld thread.\n", link->to_id,thread_res);
                //printf("    Link %ld before: %f\n", link->to_id, nodes[link->to_id].score_next);
                nodes[link->to_id].score_next += score; //need sync
                //printf("    Link %ld after (next): %f\n", link->to_id, nodes[link->to_id].score_next);

                link = link->next;
            }

            nodes[i].score_next -= nodes[i].score * D_FACTOR;
            //printf("    Node after update: %f\n", nodes[i].score_next);
        }

        pthread_barrier_wait(&bar);

        for (i = data->from; i < data->to; i++)
        {
            nodes[i].score = nodes[i].score_next;
            //nodes[i].score_next = 0;
        }
    }
    return NULL;
}