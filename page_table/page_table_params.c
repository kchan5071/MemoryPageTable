#include "page_table_params.h"

uint32_t* get_levels(char** output, int length) {
    //get the number of levels
    //argv [1-n] are the levels
    uint32_t* levels = (uint32_t*)malloc((int)sizeof(int));
    for (int i = 0; i < length; i++) {
        levels[i] = atoi(output[i + 2]);
    }
    return levels;
}

uint32_t* calculate_entry_count(uint32_t* levels, int depth) {
    uint32_t* entry_count = (uint32_t*)malloc((int)sizeof(int) * depth);
    for (int i = 0; i < depth; i++) {
        entry_count[i] = 1 << levels[i];
    }
    return entry_count;
}

uint32_t* get_page_indices(int address, uint32_t* levels, uint32_t* shifts, int depth) {
    uint32_t* indices = (uint32_t*)malloc((int)sizeof(int) * sizeof(levels));
    int shift = 0;
    shift = levels[depth - 1];
    for (int i = 0; i < depth; i++) {
        int mask = 0;
        mask = (1 << levels[i]) - 1;
        indices[i] = (address >> shifts[i]) & mask;
        shift -= levels[i];
    }
    return indices;
}

uint32_t* create_bit_masks(uint32_t* levels, int depth) {
    uint32_t* offsets = (uint32_t*)malloc((int)sizeof(int) * depth);
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

uint32_t* create_shifts(uint32_t* levels, int depth, uint32_t* bitmasks) {
    uint32_t* shifts = (uint32_t*)malloc((int)sizeof(int) * depth);
    //shift for 32int
    int shift = 32;
    //shift each level more and more
    for (int i = 0; i < depth; i++) {
        shift -= levels[i];
        shifts[i] = shift;
    }
    return shifts;
}