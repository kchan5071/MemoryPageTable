#include "page_table.h"

node* create_node(int address, int children_count) {
    node* new_node = (node*)malloc((int)sizeof(node) + (children_count * (int)sizeof(node*)));
    new_node->address = address;
    new_node->children = (node**)calloc((int)sizeof(node), children_count);
    new_node->times_accessed = 1;
    return new_node;
}

page_table* init_page_table(uint32_t* levels, uint32_t* bitmask, uint32_t* shift, uint32_t* entry_counts, int depth) {
    page_table* new_table = (page_table*)malloc((int)sizeof(page_table));
    new_table->root = create_node(0, entry_counts[0]);
    new_table->levels = levels;
    new_table->bitmask = bitmask;
    new_table->shift = shift;
    new_table->entry_count = entry_counts;
    return new_table;
}

page_table* build_page_table(char** argv, int* depth) {
    //get levels, bitmasks, shifts, and entry counts
    uint32_t* levels = get_levels(argv, depth);
    uint32_t* bitmasks = create_bit_masks(levels, *depth);
    uint32_t* shifts = create_shifts(levels,*depth, bitmasks);
    uint32_t* entry_counts = calculate_entry_count(levels, *depth);

    //create page table
    page_table* table = init_page_table(levels, bitmasks, shifts, entry_counts, *depth);
    return table;
}

// access the page table, increment the the number of times the page has been accessed
uint32_t record_page_access(page_table* table, node* root, uint32_t* page_indices, int at_level, int depth) {
    node* current = root;
    if (at_level == depth) {
        return current->times_accessed++;
    }
    uint32_t index = page_indices[at_level];

    if (current->children[index] == NULL) {
        current->children[index] = create_node(index, table->entry_count[at_level + 1]);
    }
    current = current->children[index];
    return record_page_access(table, current, page_indices, at_level + 1, depth);
}