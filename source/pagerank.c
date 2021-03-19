#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "graph/graph.h"

#define LINE_SIZE 100
#define ITERATIONS 50
#define THREADS_NUM 1
#define DEBUG 1
#define D_FACTOR 0.85
#define INIT_SCORE 1.0

typedef struct thread_data_t
{
    long from;
    long to;
} thread_data_t;

pthread_t threads[THREADS_NUM];
thread_data_t data[THREADS_NUM];

pthread_barrier_t bar;

graph_t *g;

void pagerank();
void read_file(char *filename);
void write_file(char *filename);
void *pagerank_calculate(void *arg);
double pagerank_link_sum(link_t *head);

int main(int argc, char **argv)
{
    int opt;
    char *input_filename;
    char *output_filename = "pagerank.csv";

    while ((opt = getopt(argc, argv, "f:h")) != -1)
    {
        switch (opt)
        {
        case 'f':
            input_filename = strdup(optarg);
            break;
        case 'h':
            return 0;    
        default:
            exit(EXIT_FAILURE);
        }
    }

    read_file(input_filename);

    //pagerank();

    //write_file(output_filename);

    graph_print(g);
    
    free(input_filename);
    
    graph_free(g);
    return 0;
}

void pagerank()
{
    long sets, i;

    pthread_barrier_init(&bar, NULL, THREADS_NUM);

    sets = g->size / THREADS_NUM;
    for (i = 0; i < THREADS_NUM; i++)
    {
        data[i].from = i * sets;
        if (i != THREADS_NUM - 1)
            data[i].to = (i + 1) * sets;
        else
            data[THREADS_NUM - 1].to = g->size;

        //printf("Thread %ld processes node[%ld] to node[%ld]\n", i, data[i].from, (data[i].to - 1));
        pthread_create(&threads[i], NULL, pagerank_calculate, &data[i]);
    }

    for (i = 0; i < THREADS_NUM; i++)
    {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&bar);

}

void *pagerank_calculate(void *arg)
{
    thread_data_t *data;
    node_t *nodes, *neigh;
    link_t *curr, *n_curr;
    double score;
    long i, j, from_index;

    data = (thread_data_t *)arg;
    nodes = g->nodes;

    //printf("Thread handling node[%ld] to node[%ld] started!.\n", data->from, data->to - 1);

    for (j = 0; j < ITERATIONS; j++)
    {
        for (i = data->from; i < data->to; i++)
        {
            //printf("Node %ld:\n", nodes[i].id);

            curr = nodes[i].inclinks_head;
            while (curr != NULL)
            {
                //printf("Link from %ld to %ld\n", nodes[curr->from_node_index].id, nodes[curr->to_node_index].id);

                from_index = curr->from_node_index;

                if (nodes[from_index].outlinks_num != 0) //no need, just safe
                    curr->transfer_score = nodes[from_index].score * D_FACTOR / nodes[from_index].outlinks_num;
                else
                    curr->transfer_score = 0;

                curr = curr->next;
            }
        }

        pthread_barrier_wait(&bar);
        for (i = data->from; i < data->to; i++)
        {
            score = pagerank_link_sum(nodes[i].inclinks_head);
            if (nodes[i].outlinks_num != 0)
                nodes[i].score = score + nodes[i].score * (1 - D_FACTOR);
            else
                nodes[i].score = score + nodes[i].score;
        }
        pthread_barrier_wait(&bar);
    }
}

double pagerank_link_sum(link_t *head)
{
    link_t *curr;
    double sum = 0;

    curr = head;
    while (curr != NULL)
    {
        sum += curr->transfer_score;
        curr = curr->next;
    }

    return sum;
}

void read_file(char *filename)
{
    FILE *stream;
    char line[LINE_SIZE], *tok;
    long from, to, size = 0, edges = 0, counter = 0;

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
        edges++;
    }

    if (DEBUG)
        printf("Total edges: %ld\n", edges);

    rewind(stream);

    g = graph_init(2 * edges, INIT_SCORE);

    while ((fgets(line, LINE_SIZE, stream)) != NULL)
    {
        if (line[0] == '#')
        {
            continue;
        }

        tok = strtok(line, "\r \t\v");
        from = atol(tok);

        tok = strtok(NULL, "\r \t\v");
        to = atol(tok);

        //if (DEBUG)
        //  printf("From %ld to %ld.\n", from, to);

        graph_add_link(g, from, to);
    }

    if (DEBUG)
        printf("Total nodes: %ld\n", g->size);


    graph_free_unused(g);

    fclose(stream);
}

void write_file(char *filename)
{
    FILE *stream;

    if (!(stream = fopen(filename, "w+")))
    {
        perror("Could not open the file");
        exit(EXIT_FAILURE);
    }

    graph_csv(g, stream);

    printf("File saved: \"%s\".\n", filename);
    fclose(stream);
}