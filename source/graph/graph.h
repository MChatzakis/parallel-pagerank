#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct link_t
{
    long from_node_index;
    long to_node_index;

    double transfer_score; //This is used only for incoming links!

    struct link_t *next;
} link_t;

typedef struct node_t
{
    long id;
    double score;

    long outlinks_num;
    link_t *outlinks_head;

    long inclinks_num;
    link_t *inclinks_head;
} node_t;

typedef struct graph_t
{
    long size;
    double init_score;

    node_t *nodes;
} graph_t;

graph_t *graph_init(long max_size, double init_score);
node_t *graph_add_link(graph_t *g, long from, long to);
void graph_csv(graph_t *g, FILE *stream);
void graph_print(graph_t *g);