#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct link_t;

typedef struct node_t
{
    long id;
    long out_links_num;

    double score;

    struct link_t *outlinks_head;
    struct node_t *next;
} node_t;

typedef struct link_t
{
    struct node_t *node;
    struct link_t *next;
} link_t;

typedef struct graph_t
{
    long size;
    double initial_score;
    node_t *head;
} graph_t;

graph_t *graph_initialize(double init_score)
{
    graph_t *g;

    g = (graph_t *)malloc(sizeof(graph_t));
    if (!g)
    {
        perror("Could not alocate memory for the graph");
        exit(EXIT_FAILURE);
    }

    g->head = NULL;
    g->initial_score = init_score;
    g->size = 0;

    return g;
}

node_t *graph_add_node(graph_t *g, long id)
{
    node_t *curr, *prev, *node;

    assert(g);

    curr = g->head;
    prev = NULL;

    while (curr != NULL && curr->id <= id)
    {
        if (curr->id == id)
        {
            g->size++;
            return curr;
        }

        prev = curr;
        curr = curr->next;
    }

    node = (node_t *)malloc(sizeof(node_t));

    node->id = id;
    node->next = curr;
    node->score = g->initial_score;
    node->out_links_num = 0;
    node->outlinks_head = NULL;

    g->size++;

    if (prev == NULL)
    {
        g->head = node;

        return g->head;
    }
    /*else if (curr == g->head)
    {

    }*/
    else
    {
        prev->next = node;
    }

    return node;
}

link_t *graph_add_link(graph_t *g, long from, long to)
{
    node_t *from_node, *to_node;
    link_t *curr, *prev, *link;

    assert(g);

    if (from == to)
    {
        printf("samee\n");
        return NULL;
    }

    if (!(from_node = graph_add_node(g, from)))
    {
        perror("Could not locate the node");
        exit(EXIT_FAILURE);
    }

    if (!(to_node = graph_add_node(g, to)))
    {
        perror("Could not locate the node");
        exit(EXIT_FAILURE);
    }

    curr = from_node->outlinks_head;
    prev = NULL;

    while (curr != NULL)
    {
        if ((curr->node)->id == to)
        {
            return curr;
        }
        prev = curr;
        curr = curr->next;
    }

    link = (link_t *)malloc(sizeof(link_t));
    link->next = curr;
    link->node = to_node;

    if (prev == NULL)
    {
        from_node->outlinks_head = link;
    }
    else
    {
        prev->next = link;
    }

    from_node->out_links_num++;

    return link;
}

void graph_print_nodes(graph_t *g)
{
    node_t *curr;
    link_t *link;

    assert(g);

    curr = g->head;

    printf("---Graph consisting of %ld nodes---\n", g->size);

    while (curr)
    {
        printf("Node %ld\n", curr->id);
        printf("    Pagerank: %.2f\n", curr->score);
        printf("    Out Deegree: %ld\n      =>[ ", curr->out_links_num);
        link = curr->outlinks_head;
        while (link != NULL)
        {
            printf("%ld ", link->node->id);
            link = link->next;
        }
        printf("]\n");
        curr = curr->next;
    }
}

void graph_output(graph_t *g, FILE *stream)
{
    node_t *curr;
    link_t *link;

    assert(g);

    curr = g->head;

    while (curr)
    {
        fprintf(stream, "%ld,%.2f\n", curr->id, curr->score);
        curr = curr->next;
    }
}
