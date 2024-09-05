#include "page_table_params.h"

/*
* takes argv and depth of table as an input

* returns an array of levels
* levels are the number of bits used to index each level of the page table
*
*/
uint32_t* get_levels(char** argv, int* depth) {
    // *depth = (int)(sizeof(argv[2]) / sizeof(char)) / 2 - 1;
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

uint32_t* calculate_entry_count(uint32_t* levels, int depth) {
    uint32_t* entry_count = (uint32_t*)calloc((int)sizeof(int), depth);
    for (int i = 0; i < depth; i++) {
        entry_count[i] = 1 << levels[i];
    }
    return entry_count;
}

uint32_t* get_page_indices(int address, uint32_t* bitmasks, uint32_t* shifts, int depth) {
    uint32_t* indices = (uint32_t*)calloc((int)sizeof(int), depth);
    for (int i = 0; i < depth; i++) {
        indices[i] = extract_page_number_from_address(address, bitmasks[i], shifts[i]);
    }
    return indices;
}

uint32_t extract_page_number_from_address(uint32_t address, uint32_t bitmask, uint32_t shift) {
    return (address & bitmask) >> shift;
}

uint32_t* create_bit_masks(uint32_t* levels, int depth) {
    uint32_t* offsets = (uint32_t*)calloc((int)sizeof(int), depth);
    int offset = 32;
    for (int i = 0; i < depth; i++) {
        offset -= levels[i];
        offsets[i] = 0;
        for (int j = 0; j < levels[i]; j++) {
            offsets[i] |= 1 << (offset + j);
        }
    }
    return offsets;
}

uint32_t* create_shifts(uint32_t* levels, int depth) {
    uint32_t* shifts = (uint32_t*)calloc((int)sizeof(int), depth);
    uint32_t shift = 32;
    for (int i = 0; i < depth; i++) {
        shift -= levels[i];
        shifts[i] = shift;
    }
    return shifts;
}