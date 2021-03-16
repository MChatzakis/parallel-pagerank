#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct link_t
{
    long from_node_index;
    long from_node_id;

    long to_node_index;
    long to_node_id;

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

graph_t *graph_init(long max_size, double init_score)
{
    graph_t *g;
    long i;

    g = (graph_t *)malloc(sizeof(graph_t));

    /* Max size is 2*edges, as known from graph theory */
    g->nodes = (node_t *)malloc(max_size * sizeof(node_t));
    g->size = 0;
    g->init_score = init_score;
    return g;
}

node_t *graph_add_link(graph_t *g, long from, long to)
{
    short from_exists = 0, to_exists = 0;
    long i, from_index, to_index; //check -1!!!!!
    node_t *nodes;
    link_t *curr, *prev, *link;

    assert(g);

    nodes = g->nodes;
    for (i = 0; i < g->size; i++)
    {
        if (nodes[i].id == from)
        {
            from_exists = 1;
            from_index = i;
        }

        if (nodes[i].id == to)
        {
            to_exists = 1;
            to_index = i;
        }

        if (from_exists && to_exists)
        {
            break; //Speedup operation
        }
    }

    if (!from_exists)
    {
        nodes[g->size].id = from;
        nodes[g->size].score = g->init_score;
        nodes[g->size].outlinks_num = 0;
        nodes[g->size].inclinks_num = 0;
        nodes[g->size].outlinks_head = NULL;
        nodes[g->size].inclinks_head = NULL;

        from_index = g->size;
        g->size++;
    }

    if (!to_exists)
    {
        nodes[g->size].id = to;
        nodes[g->size].score = g->init_score;
        nodes[g->size].outlinks_num = 0;
        nodes[g->size].inclinks_num = 0;
        nodes[g->size].outlinks_head = NULL;
        nodes[g->size].inclinks_head = NULL;

        to_index = g->size;
        g->size++;
    }

    //at this point, the 2 nodes are nodes[from_index] and nodes[to_index]

    //add outgoing link to from_index
    curr = nodes[from_index].outlinks_head;
    prev = NULL;
    while (curr != NULL)
    {
        prev = curr;
        curr = curr->next;
    }

    link = (link_t *)malloc(sizeof(link_t));
    link->from_node_id = from;
    link->from_node_index = from_index;
    link->to_node_id = to;
    link->to_node_index = to_index;
    link->transfer_score = 0.0; //This wont be used anyways
    link->next = NULL;

    if (prev == NULL)
    {
        nodes[from_index].outlinks_head = link;
    }
    else
    {
        prev->next = link;
    }

    nodes[from_index].outlinks_num++;

    /////////////////////////////////////////////
    //adding to node_to the various stuff..
    curr = nodes[to_index].inclinks_head;
    prev = NULL;
    while (curr != NULL)
    {
        prev = curr;
        curr = curr->next;
    }

    link = (link_t *)malloc(sizeof(link_t));
    link->from_node_id = from;
    link->from_node_index = from_index;
    link->to_node_id = to;
    link->to_node_index = to_index;
    link->transfer_score = 0.0;
    link->next = NULL;

    if (prev == NULL)
    {
        nodes[to_index].inclinks_head = link;
    }
    else
    {
        prev->next = link;
    }

    nodes[to_index].inclinks_num++;

    return nodes;
}

void graph_print(graph_t *g)
{
    node_t *nodes;
    link_t *link;
    long i;

    assert(g);

    nodes = g->nodes;

    printf("Graph of %ld nodes.\n", g->size);
    for (i = 0; i < g->size; i++)
    {
        printf("Node ID: %ld\n", nodes[i].id);
        printf("    Pagerank: %f\n", nodes[i].score);
        printf("    Outdegree: %ld\n        => [ ", nodes[i].outlinks_num);
        link = nodes[i].outlinks_head;
        while (link != NULL)
        {
            //printf("%ld ", link->to_node->id);
            printf("%ld ", nodes[link->to_node_index].id);
            link = link->next;
        }
        printf("]\n");

        printf("    Incdegree: %ld\n        => [ ", nodes[i].inclinks_num);
        link = nodes[i].inclinks_head;
        while (link != NULL)
        {
            //printf("%ld ", link->to_node->id);
            printf("%ld ", nodes[link->from_node_index].id);
            link = link->next;
        }
        printf("]\n");
    }
}
