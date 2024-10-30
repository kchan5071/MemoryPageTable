#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "page_table_params.h"

/**
 * Kai Chan
 * 827673009
 *
 * Halie Do
 * 827707836
 */

// map struct containing page info
typedef struct map
{
    uint32_t address;  // virtual page number
    int time_accessed; // the time the page was most recently accessed
    int frame_number;  // frame number associated with the page in the page table
} map;

// tree node struct
typedef struct node
{
    struct node **children; // array of children
    map *page_info;         // contains info about the page
} node;

// Page table struct
typedef struct page_table
{
    node *root;
    uint32_t *levels;
    uint32_t *bitmask;     // bitmask for each level
    uint32_t *shift;       // shift for each level
    uint32_t *entry_count; // number of entries for each level
    int num_of_entries;

} page_table;

// create a new node
node *create_node(int address, int children_count);

// create a new page table object with given parameters
page_table *init_page_table(uint32_t *levels, uint32_t *bitmask, uint32_t *shift, uint32_t *entry_counts, int depth);

// build a page table from the given arguments(calls create_page_table)
page_table *build_page_table(char **argv, int *depth, uint32_t *depth_array);

// look up and return a pointer to map struct containing info of target page
map *lookup_vpn2pfn(page_table *table, node *root, uint32_t *page_indices, int at_level, int depth);
// insert new page in the page table
void insert_vpn2pfn(page_table *table, node *root, uint32_t *page_indices, int at_level, int depth, int time_accessed, int *frame, uint32_t vpn);

#endif
