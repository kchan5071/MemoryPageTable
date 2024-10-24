#include "page_table.h"

/**
 * Kai Chan 
 * 827673009
 */

/**
 * @brief: create a new node
 * 
 * @param address: the address of the node
 * @param children_count: the number of children the node has
 * 
 * @return: the new node
 */
node* create_node(int address, int children_count) {
    node* new_node = (node*)malloc((int)sizeof(node) + (children_count * (int)sizeof(node*)));
    new_node->address = address;
    //initialize children array of size children_count
    new_node->children = (node**)calloc((int)sizeof(node), children_count);
    new_node->times_accessed = 1;
    return new_node;
}

/**
 * @brief: helper function to build_page_table to initialize the page table after getting the parameters
 * 
 * @param levels: the number of bits at each level of the page table
 * @param bitmask: the bitmask for each level
 * @param shift: the shift for each level
 * @param entry_counts: the number of entries for each level
 * @param depth: the number of levels in the page table
 * 
 * @return: the page table
 */
page_table* init_page_table(uint32_t* levels, uint32_t* bitmask, uint32_t* shift, uint32_t* entry_counts, int depth) {
    //create new page table
    page_table* new_table = (page_table*)malloc((int)sizeof(page_table));
    new_table->root = create_node(0, entry_counts[0]);
    new_table->levels = levels;
    new_table->bitmask = bitmask;
    new_table->shift = shift;
    new_table->entry_count = entry_counts;
    return new_table;
}

/**
 * @brief: build a page table from the given arguments
 * 
 * @param argv: the arguments to build the page table from
 * @param depth: the number of levels in the page table
 * 
 * @return: the page table
 */
page_table* build_page_table(char** argv, int* depth) {
    //get parameters
    uint32_t* levels = get_levels(argv, depth);
    uint32_t* bitmasks = create_bit_masks(levels, *depth);
    uint32_t* shifts = create_shifts(levels, *depth);
    uint32_t* entry_counts = calculate_entry_count(levels, *depth);

    //create page table
    page_table* table = init_page_table(levels, bitmasks, shifts, entry_counts, *depth);
    return table;
}

/**
 * @brief: record the page access in the page table
 * 
 * @param table: the page table to record the access in
 * @param root: the root of the page table
 * @param page_indices: the page indices to record the access for
 * @param at_level: the current level in the page table
 * @param depth: the number of levels in the page table
 * 
 * @return: the number of times the page has been accessed
 */
uint32_t record_page_access(page_table* table, node* root, uint32_t* page_indices, int at_level, int depth) {
    //base case
    node* current = root;
    if (at_level == depth) {
        return current->times_accessed++;
    }
    //recursive case
    uint32_t index = page_indices[at_level];
    //create new node if it doesn't exist
    if (current->children[index] == NULL) {
        current->children[index] = create_node(index, table->entry_count[at_level + 1]);
    }
    //traverse to next level
    current = current->children[index];
    return record_page_access(table, current, page_indices, at_level + 1, depth);
}