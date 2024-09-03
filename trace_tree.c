//creates a tree from the trace file
#include <stdio.h>
#include <stdlib.h>
#include "tracereader.h"
#include "log.h"
#include "page_table.h"

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

page_table* build_page_table(char** argv, int depth) {
    //get levels, bitmasks, shifts, and entry counts
    uint32_t* levels = get_levels(argv, depth);
    uint32_t* bitmasks = create_bit_masks(levels, depth);
    uint32_t* shifts = create_shifts(levels, depth, bitmasks);
    uint32_t* entry_counts = calculate_entry_count(levels, depth);

    //create page table
    page_table* table = create_page_table(levels, bitmasks, shifts, entry_counts, depth);
    return table;
}

int main(int argc, char** argv) {

    //read args
    char *filename = argv[1];
    int depth = argc - 2;

    //create page table
    page_table* table = build_page_table(argv, depth);

    //log bitmasks
    log_bitmasks(depth, table->bitmask);

    //create trace file and trace struct
    FILE *trace_file = fopen(filename, "r");
    p2AddrTr trace;

    //loop through all addresses
    int address = -1;
    while (address != 0) {
        address = NextAddress(trace_file, &trace);
        uint32_t* indices = get_page_indices(trace.addr, table->levels, table->shift, depth);
        int times_accessed = record_page_access(table, table->root, indices, 0, depth);
        //log accesses
        log_pgindices_numofaccesses(trace.addr, depth, indices, times_accessed);
    }
    fclose(trace_file);
    return 0;
}