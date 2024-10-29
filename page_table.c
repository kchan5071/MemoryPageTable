#include "page_table.h"

/**
 * Kai Chan
 * 827673009
 *
 * Halie Do
 * 827707836
 */

/**
 * @brief: create a new node
 *
 * @param address: the address of the node
 * @param children_count: the number of children the node has
 *
 * @return: the new node
 */
node *create_node(int address, int children_count)
{
    node *new_node = (node *)malloc((int)sizeof(node) + (children_count * (int)sizeof(node *)));
    // // initialize children array of size children_count
    new_node->children = (node **)calloc((int)sizeof(node), children_count);
    new_node->page_info = (map *)malloc(sizeof(map));
    new_node->page_info->address = address;
    new_node->page_info->frame_number = -1;
    new_node->page_info->time_accessed = -1;

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
page_table *init_page_table(uint32_t *levels, uint32_t *bitmask, uint32_t *shift, uint32_t *entry_counts, int depth)
{
    // create new page table
    page_table *new_table = (page_table *)malloc((int)sizeof(page_table));
    new_table->root = create_node(0, entry_counts[0]);
    new_table->levels = levels;
    new_table->bitmask = bitmask;
    new_table->shift = shift;
    new_table->entry_count = entry_counts;
    new_table->num_of_entries = entry_counts[0];
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
page_table *build_page_table(char **argv, int *depth, uint32_t *depth_array)
{
    // get parameters
    uint32_t *bitmasks = create_bit_masks(depth_array, *depth);
    uint32_t *shifts = create_shifts(depth_array, *depth);
    uint32_t *entry_counts = calculate_entry_count(depth_array, *depth);

    // create page table
    page_table *table = init_page_table(depth_array, bitmasks, shifts, entry_counts, *depth);
    return table;
}

// TODO: fix docs
/**
 * @brief: look up the page access in the page table
 *
 * @param table: the page table to record the access in
 * @param root: the root of the page table
 * @param page_indices: the page indices to record the access for
 * @param at_level: the current level in the page table
 * @param depth: the number of levels in the page table
 *
 * @return: the info of page if it was in the table
 */
map *lookup_vpn2pfn(page_table *table, node *root, uint32_t *page_indices, int at_level, int depth)
{
    // base case
    node *current = root;
    if (at_level == depth)
    {
        if (current->page_info->frame_number != -1)
            return current->page_info;
    }
    // TODO: fix later
    //  recursive case
    uint32_t index = page_indices[at_level];
    if (current->children[index] == NULL)
    {
        return NULL;
    }
    // traverse to next level
    current = current->children[index];
    return lookup_vpn2pfn(table, current, page_indices, at_level + 1, depth);
}

/**
 * @brief: record the page access in the page table
 *
 * @param table: the page table to record the access in
 * @param root: the root of the page table
 * @param page_indices: the page indices to record the access for
 * @param at_level: the current level in the page table
 * @param depth: the number of levels in the page table
 * @param time_accessed: time the page was accessed
 * @param frame: pointer to variable holding frame number for the address
 * @param vpn: virtual page number extracted from the read address
 *
 */
void insert_vpn2pfn(page_table *table, node *root, uint32_t *page_indices, int at_level, int depth, int time_accessed, int *frame, uint32_t vpn)
{
    // base case
    node *current = root;
    if (at_level == depth)
    {
        // check if the page has been accessed before
        if (current->page_info->time_accessed == -1)
        {
            current->page_info->address = vpn;
            current->page_info->frame_number = *frame;
            ++(*frame);
        }
        current->page_info->time_accessed = time_accessed;
        return;
    }
    // recursive case
    uint32_t index = page_indices[at_level];
    // create new node if it doesn't exist
    if (current->children[index] == NULL)
    {
        current->children[index] = create_node(index, table->entry_count[at_level + 1]);
        table->num_of_entries += table->entry_count[at_level + 1];
    }
    // traverse to next level
    current = current->children[index];
    insert_vpn2pfn(table, current, page_indices, at_level + 1, depth, time_accessed, frame, vpn);
}
