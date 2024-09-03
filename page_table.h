#include <stdio.h>
#include <stdlib.h>

//tree node struct
typedef struct node {
    int address;
    int times_accessed;
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

//create a new node
node* create_node(int address, int children_count);

//build the tree
node* build_tree(page_table* table, int address, int depth);

//create a new page table
page_table* create_page_table(uint32_t* levels, uint32_t* bitmask, uint32_t* shift, uint32_t* entry_counts, int depth);

//access the page table, increment the the number of times the page has been accessed
//naming different due to 
uint32_t record_page_access(page_table* table, node* root, uint32_t* page_indices, int at_level, int depth);



