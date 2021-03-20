/**
 * @file graph.h
 * @author Manos Chatzakis (chatzakis@ics.forth.gr)
 * @brief A simple graph library
 * @date 2021-03-19
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/**
 * @brief Directed Link abstraction
 * 
 */
typedef struct link_t
{
    long from_node_index;
    long to_node_index;

    double transfer_score;

    struct link_t *next;
} link_t;

/**
 * @brief Graph Node abstraction
 * 
 */
typedef struct node_t
{
    long id;
    double score;
    double score_add;

    long outlinks_num;
    link_t *outlinks_head;

    long inclinks_num;
    link_t *inclinks_head;
} node_t;

/**
 * @brief Graph abtraction
 * 
 */
typedef struct graph_t
{
    long size;
    double init_score;

    node_t *nodes;
} graph_t;

/**
 * @brief Creates a new empty graph
 * 
 * @param max_size Max possible node (2*edges)
 * @param init_score The initial pagerank of each node
 * @return graph_t* Pointer to the new graph
 */
graph_t *graph_init(long max_size, double init_score);

/**
 * @brief Frees the unused memory block allocated for the node array.
 * 
 * @param g The graph
 * @return node_t* Pointer to the node array
 */
node_t *graph_free_unused(graph_t *g);

/**
 * @brief Adds a link from node from to node to. If the nodes are not inside 
 * the array, they are added first.
 * 
 * @param g The directed graph
 * @param from From node ID
 * @param to To node ID
 * @return node_t* Pointer to the node array
 */
node_t *graph_add_link(graph_t *g, long from, long to);

/**
 * @brief Writes the csv representation of the data to the specified stream.
 * 
 * @param g The graph containing the data
 * @param stream The output stream
 */
void graph_csv(graph_t *g, FILE *stream);

/**
 * @brief Prints the whole graph in a readable way to stdout.
 * 
 * @param g The graph to be printed
 */
void graph_print(graph_t *g);

/**
 * @brief Frees the memory allocated for the graph.
 * 
 * @param g The graph pointer to the memory allocated
 */
void graph_free(graph_t *g);