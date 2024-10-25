#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "page_table_params.h"

/**
 * Kai Chan 
 * 827673009
 */

//tree node struct
typedef struct node {
    int address;
    int time_accessed;
    //array of children
    struct node** children;
} node;

//Page table struct
typedef struct page_table {
    node* root;
    uint32_t* levels;
    uint32_t* bitmask; //bitmask for each level
    uint32_t* shift; //shift for each level
    uint32_t* entry_count; //number of entries for each level
} page_table;

typedef struct address_time_pair {
    uint32_t address;
    int time_accessed;
} address_time_pair;


//create a new node
node* create_node(int address, int children_count);

//create a new page table object with given parameters
page_table* init_page_table(uint32_t* levels, uint32_t* bitmask, uint32_t* shift, uint32_t* entry_counts, int depth);

//build a page table from the given arguments(calls create_page_table)
page_table* build_page_table(char** argv, int* depth, int* depth_array);

address_time_pair record_page_access(page_table* table, node* root, uint32_t* page_indices, int at_level, int depth, int time_accessed);



