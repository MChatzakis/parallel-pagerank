#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct link_t
{
    long to_id;
    struct link_t *next;
} link_t;

typedef struct node_t
{
    long id;
    long outlinks_num;

    long *outlinks;
    link_t *outlinks_head;

    double score;
} node_t;

typedef struct graph_t
{
    long size;
    node_t *nodes;
} graph_t;

graph_t *graph_initialize(double starting_score, long size)
{
    graph_t *g;
    long i;

    g = (graph_t *)malloc(sizeof(graph_t));
    if (!g)
    {
        perror("Could not allocate memory for graph");
        exit(EXIT_FAILURE);
    }

    g->size = size;
    g->nodes = (node_t *)malloc(size * sizeof(node_t));
    if (!(g->nodes))
    {
        perror("Could not allocate memory for nodes");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < size; i++)
    {
        g->nodes[i].id = i;
        g->nodes[i].outlinks = 0;
        g->nodes[i].score = starting_score;
        //g->nodes[i].outlinks = (long *)malloc((size - 1) * sizeof(long));
    }

    return g;
}

link_t *graph_add_link(graph_t *g, long from, long to)
{
    node_t *nodes;
    link_t *curr, *prev, *link;
    assert(g && g->nodes);

    if (from == to || to >= g->size || from < 0)
    {
        fprintf(stderr, "Link values are wrong [%ld, %ld].\n", from, to);
        exit(EXIT_FAILURE);
    }

    nodes = g->nodes;

    curr = nodes[from].outlinks_head;
    prev = NULL;
    while (curr != NULL)
    {
        prev = curr;
        curr = curr->next;
    }

    link = (link_t *)malloc(sizeof(link_t));
    if (!link)
    {
        perror("Could not allocate memory for a new link");
        exit(EXIT_FAILURE);
    }

    link->to_id = to;
    link->next = NULL;

    if (prev == NULL)
    {
        nodes[from].outlinks_head = link;
    }
    else
    {
        prev->next = link;
    }

    nodes[from].outlinks_num++;
    return link;
}

void graph_add_link_l(graph_t *g, long from, long to)
{
    node_t *nodes;
    assert(g && g->nodes);

    if (from == to || to >= g->size || from < 0)
    {
        fprintf(stderr, "Link values are wrong [%ld, %ld].\n", from, to);
        exit(EXIT_FAILURE);
    }

    nodes = g->nodes;

    nodes[from].outlinks[nodes[from].outlinks_num] = to;
    nodes[from].outlinks_num++;
}

void graph_print(graph_t *g, FILE *stream)
{
    node_t *nodes;
    link_t *link;
    long i, j;
    assert(g && g->nodes);

    nodes = g->nodes;
    fprintf(stream, "Graph of %ld nodes.\n", g->size);
    for (i = 0; i < g->size; i++)
    {
        fprintf(stream, "Node: %ld\n", nodes[i].id);
        fprintf(stream, "    Outdegree: %ld\n        [ ", nodes[i].outlinks_num);
        /*for (j = 0; j < nodes[i].outlinks_num; j++)
        {
            fprintf(stream, "%ld ", nodes[i].outlinks[j]);
        }*/
        link = nodes[i].outlinks_head;
        while (link != NULL)
        {
            printf("%ld ", link->to_id);
            link = link->next;
        }
        fprintf(stream, "]\n");
        fprintf(stream, "    Pagerank: %.2f\n", nodes[i].score);
    }
}

void graph_csv(graph_t *g, FILE *stream)
{
    node_t *nodes;
    long i, j;
    assert(g && g->nodes);

    nodes = g->nodes;
    for (i = 0; i < g->size; i++)
    {
        fprintf(stream, "%ld,", nodes[i].id);
        fprintf(stream, "%.2f\n", nodes[i].score);
    }
}

void graph_free(graph_t *g, FILE *stream)
{
}