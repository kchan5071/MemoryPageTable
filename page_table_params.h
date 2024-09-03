#include <stdlib.h>
#include <stdio.h>

uint32_t* get_levels(char** output, int length);

uint32_t* calculate_entry_count(uint32_t* levels, int depth);

uint32_t* get_page_indices(int address, uint32_t* levels, uint32_t* shifts, int depth);

uint32_t* create_bit_masks(uint32_t* levels, int depth);

uint32_t* create_shifts(uint32_t* levels, int depth, uint32_t* bitmasks);