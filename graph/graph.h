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
    double score_next;
} node_t;

typedef struct graph_t
{
    long size;
    node_t *nodes;
} graph_t;

graph_t *graph_initialize(double starting_score, long size);
link_t *graph_add_link(graph_t *g, long from, long to);
void graph_add_link_l(graph_t *g, long from, long to);
void graph_print(graph_t *g, FILE *stream);
void graph_csv(graph_t *g, FILE *stream);
void graph_free(graph_t *g, FILE *stream);