#include "page_table_params.h"


#define BIT_SIZE 32
/** 
 * @brief: get the levels of the page table from the command line arguments
 * 
* @param argv: the command line arguments
* @param depth: pointer to the number of levels in the page table
* 
* @return: array that stores the number of bits at each level of the page table
*/
uint32_t* get_levels(char** argv, int* depth) {
    char** token_character_array = (char**)calloc((int)sizeof(int), 32 * 4);
    char* level_char = argv[2];
    char* token = strtok(level_char, " ");
    int level_count = 0;
    while (token != NULL) {
        token_character_array[level_count] = (token);
        token = strtok(NULL, " ");
        level_count++;
    }
    *depth = level_count;

    uint32_t* levels = (uint32_t*)calloc((int)sizeof(int), *depth);
    for (int j = 0; j < *depth; j++) {
        levels[j] = atoi(token_character_array[j]);
    }
    return levels;
}

/**
 * @brief: calculate the number of entries at each level of the page table
 * 
 * @param levels: pointer to array that stores the number of bits at each level of the page table
 * @param depth: the number of levels in the page table
 * 
 * @return: array that stores the number of entries at each level of the page table
 */
uint32_t* calculate_entry_count(uint32_t* levels, int depth) {
    uint32_t* entry_count = (uint32_t*)calloc((int)sizeof(int), depth);
    for (int i = 0; i < depth; i++) {
        entry_count[i] = 1 << levels[i];
    }
    return entry_count;
}

/**
 * @brief: extract the page indices from the address
 * 
 * @param address: the address to extract the page indices from
 * @param bitmasks: pointer to array that stores the bitmasks for each level of the page table
 * @param shifts: pointer to array that stores the shifts for each level of the page table
 * @param depth: the number of levels in the page table
 * 
 * @return: array that stores the page indices
 */
uint32_t* get_page_indices(int address, uint32_t* bitmasks, uint32_t* shifts, int depth) {
    uint32_t* indices = (uint32_t*)calloc((int)sizeof(int), depth);
    for (int i = 0; i < depth; i++) {
        indices[i] = extract_page_number_from_address(address, bitmasks[i], shifts[i]);
    }
    return indices;
}

/**
 * @brief: extract the page number from the address given the input
 * 
 * @param address: the address to extract the page number from
 * @param bitmask: the bitmask to apply to the address
 * @param shift: the shift to apply to the address
 * 
 * @return: the page number index
 */
uint32_t extract_page_number_from_address(uint32_t address, uint32_t bitmask, uint32_t shift) {
    return (address & bitmask) >> shift;
}

/**
 * @brief: create the bitmasks for each level of the page table
 * 
 * @param levels: pointer to array that stores the number of bits at each level of the page table
 * @param depth: the number of levels in the page table
 * 
 * @return: array that stores the bitmasks for each level of the page table
 */
uint32_t* create_bit_masks(uint32_t* levels, int depth) {
    uint32_t* offsets = (uint32_t*)calloc((int)sizeof(int), depth);
    int offset = BIT_SIZE;
    for (int i = 0; i < depth; i++) {
        offset -= levels[i];
        offsets[i] = 0;
        for (int j = 0; j < levels[i]; j++) {
            offsets[i] |= 1 << (offset + j);
        }
    }
    return offsets;
}

/**
 * @brief: create the shifts for each level of the page table
 * 
 * @param levels: pointer to array that stores the number of bits at each level of the page table
 * @param depth: the number of levels in the page table
 * 
 * @return: array that stores the shifts for each level of the page table
 */
uint32_t* create_shifts(uint32_t* levels, int depth) {
    uint32_t* shifts = (uint32_t*)calloc((int)sizeof(int), depth);
    uint32_t shift = BIT_SIZE;
    for (int i = 0; i < depth; i++) {
        shift -= levels[i];
        shifts[i] = shift;
    }
    return shifts;
}