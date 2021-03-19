/**
 * @file graph.c
 * @author Manos Chatzakis (chatzakis@ics.forth.gr)
 * @brief Graph library implementation
 * @date 2021-03-19
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "graph.h"

graph_t *graph_init(long max_size, double init_score)
{
    graph_t *g;
    long i;

    g = (graph_t *)malloc(sizeof(graph_t));
    if (!g)
    {
        perror("Could not allocate memory for graph");
        exit(EXIT_FAILURE);
    }

    /* Max size is 2*edges, as known from graph theory */
    g->nodes = (node_t *)malloc(max_size * sizeof(node_t));
    if (!(g->nodes))
    {
        perror("Could not allocate memory fot the graph nodes");
        exit(EXIT_FAILURE);
    }

    g->size = 0;
    g->init_score = init_score;

    return g;
}

node_t *graph_free_unused(graph_t *g)
{
    assert(g);

    /*g->size indicated how many items are actually inside the array*/
    g->nodes = (node_t *)realloc(g->nodes, g->size * sizeof(node_t));
    if (!(g->nodes))
    {
        perror("Could not free up additional graph space");
        exit(EXIT_FAILURE);
    }

    return g->nodes;
}

node_t *graph_add_link(graph_t *g, long from, long to)
{
    short from_exists = 0, to_exists = 0;
    long i, from_index, to_index;
    node_t *nodes;
    link_t *curr, *prev, *link;

    assert(g);

    /*Step one: Traverse the node array to find if the src and dst exist*/
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

    /*If any of src and dst does not exist, it is added to the array*/
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

    /*
        At this point, both src and dst exist
        Thus, src is added to the incoming links of dst
        and dst is added to the outgoing links of src
    */
    curr = nodes[from_index].outlinks_head;
    prev = NULL;
    while (curr != NULL)
    {
        prev = curr;
        curr = curr->next;
    }

    link = (link_t *)malloc(sizeof(link_t));
    if (!link)
    {
        perror("Could not add a new link");
        exit(EXIT_FAILURE);
    }
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
    if (!link)
    {
        perror("Could not add a new link");
        exit(EXIT_FAILURE);
    }

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
        fprintf(stream, "%f\n", nodes[i].score); /*3 decimals points are needed*/
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

void graph_free(graph_t *g)
{
    node_t *nodes;
    link_t *f_link, *link;
    long i;

    assert(g);

    /*
        For every node[i], the incoming and outgoing link list is freed
    */
    nodes = g->nodes;
    for (i = 0; i < g->size; i++)
    {
        link = nodes[i].inclinks_head;
        while (link != NULL)
        {
            f_link = link;
            link = link->next;
            free(f_link);
        }

        nodes[i].inclinks_head = NULL;

        link = nodes[i].outlinks_head;
        while (link != NULL)
        {
            f_link = link;
            link = link->next;
            free(f_link);
        }

        nodes[i].outlinks_head = NULL;
    }

    free(g->nodes);
    g->nodes = NULL;

    free(g);
}
