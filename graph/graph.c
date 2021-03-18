#include "graph.h"

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
    long i, from_index, to_index;
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
            break;
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

    curr = nodes[from_index].outlinks_head;
    prev = NULL;
    while (curr != NULL)
    {
        prev = curr;
        curr = curr->next;
    }

    link = (link_t *)malloc(sizeof(link_t));
    link->from_node_index = from_index;
    link->to_node_index = to_index;
    link->transfer_score = 0.0;
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

    curr = nodes[to_index].inclinks_head;
    prev = NULL;
    while (curr != NULL)
    {
        prev = curr;
        curr = curr->next;
    }

    link = (link_t *)malloc(sizeof(link_t));
    link->from_node_index = from_index;
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

void graph_csv(graph_t *g, FILE *stream)
{
    node_t *nodes;
    link_t *link;
    long i;

    assert(g);

    nodes = g->nodes;

    fprintf(stream, "node,pagerank\n");
    for (i = 0; i < g->size; i++)
    {
        fprintf(stream, "%ld,", nodes[i].id);
        fprintf(stream, "%f\n", nodes[i].score);
    }
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
            printf("%ld ", nodes[link->to_node_index].id);
            link = link->next;
        }
        printf("]\n");
        printf("    Incdegree: %ld\n        => [ ", nodes[i].inclinks_num);

        link = nodes[i].inclinks_head;
        while (link != NULL)
        {
            printf("%ld ", nodes[link->from_node_index].id);
            link = link->next;
        }

        printf("]\n");
    }
}
