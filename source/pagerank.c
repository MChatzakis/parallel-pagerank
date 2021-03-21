/**
 * @file pagerank.c
 * @author Manos Chatzakis (chatzakis@ics.forth.gr)
 * @brief Parallel pagerank implementation using POSIX threads
 * @date 2021-03-19
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "graph/graph.h"

#define LINE_SIZE 100   /*Max line size of the input dataset*/
#define ITERATIONS 50   /*Pagerank Iterations*/
#define TOTAL_THREADS 4 /*Max number of threads to be used is 4*/
#define DEBUG 1         /*Enable of not debugging message prompts*/
#define D_FACTOR 0.85   /*Dumping Factor*/
#define INIT_SCORE 1.0  /*Initial pagerank score*/

/**
 * @brief Struct of thread data, to be passed to the thread function.
 * 
 */
typedef struct thread_data_t
{
    long from;
    long to;
} thread_data_t;

pthread_t threads[TOTAL_THREADS];
thread_data_t data[TOTAL_THREADS];

pthread_barrier_t bar; /*Barrier to syncronize the threads to begin every iteration together*/

int THREADS_NUM = 1; /* Threads to be used */
graph_t *g;          /* Web graph simulation */

/**
 * @brief Splits the graph and creates the threads to begin
 * the pagerank calculation
 * 
 */
void pagerank();

/**
 * @brief Reads the input datasets and initializes the graph.
 * @param filename The dataset filename
 * 
 */
void read_file(char *filename);

/**
 * @brief Writes the graph data to a file
 * 
 * @param filename The filename desired. It should have a ".csv" extension
 */
void write_file(char *filename);

/**
 * @brief Every thread runs this function to calculate the pagerank of 
 * each node belonging to its corresponding pagerank pie.
 * 
 * @param arg The thread_data argument
 * @return void* NULL
 */
void *pagerank_calculate(void *arg);

/**
 * @brief Traverses the neighbour list of a node and returns the sum of the
 * score taken by every neighbour node.
 * 
 * @param head The head of the list
 * @return double The sum taken from the neighbours
 */
double pagerank_link_sum(link_t *head);

/**
 * @brief -f stands for the filename, -h stands for help.
 *
 */
int main(int argc, char **argv)
{
    int opt, count_time = 0, print_graph = 0;
    char *input_filename, *output_filename = "pagerank.csv";
    struct timespec start, finish;
    double elapsed;

    while ((opt = getopt(argc, argv, "f:t:mgh")) != -1)
    {
        switch (opt)
        {
        case 'f':
            input_filename = strdup(optarg);
            break;
        case 't':
            THREADS_NUM = atoi(optarg);
            if (THREADS_NUM <= 0 || THREADS_NUM > 4)
            {
                printf("The thread number should be between 1 and 4!\n");
                exit(EXIT_FAILURE);
            }
            printf("Number of threads: %d\n", THREADS_NUM);
            break;
        case 'm':
            count_time = 1;
            break;
        case 'g':
            print_graph = 1;
            break;
        case 'h':
            printf(
                "Usage: ./page_rank -f file -t threads [-m]\n"
                "Options:\n"
                "   -f <string>         Specifies the filename of the dataset.\n"
                "   -t <int>            Determines how many threads the algorithm will use. Must be in range of [1,4].\n"
                "   -m                  When it is used, displays the time metrics about pagerank.\n"
                "   -g                  When -g is set, the graph structure is printed to stdout.\n"
                "   -h                  Prints this help\n");
            return 0;
        default:
            exit(EXIT_FAILURE);
        }
    }

    read_file(input_filename);

    pthread_barrier_init(&bar, NULL, THREADS_NUM);

    clock_gettime(CLOCK_MONOTONIC, &start);
    pagerank();
    clock_gettime(CLOCK_MONOTONIC, &finish);

    pthread_barrier_destroy(&bar);

    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

    if (print_graph)
        graph_print(g);

    if (count_time)
        printf("Pagerank Calculation %fs\n", elapsed);

    write_file(output_filename);

    free(input_filename);
    graph_free(g);
    return 0;
}

void pagerank()
{
    long sets, i;

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
                from_index = curr->from_node_index;
                nodes[i].score_add += nodes[from_index].score * D_FACTOR / nodes[from_index].outlinks_num;
                curr = curr->next;
            }
        }

        pthread_barrier_wait(&bar);
        for (i = data->from; i < data->to; i++)
        {
            score = nodes[i].score_add;
            if (nodes[i].outlinks_num != 0)
                nodes[i].score = score + nodes[i].score * (1 - D_FACTOR);
            else
                nodes[i].score = score + nodes[i].score;

            nodes[i].score_add = 0.0;
        }
        pthread_barrier_wait(&bar);
    }
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