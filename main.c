//creates a tree from the trace file
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "lib/tracereader.h"
#include "lib/log.h"
#include "page_table/page_table.h"

int main(int argc, char** argv) {

    //read args
    char *filename = argv[1];
    int depth = 0;

    //create page table
    page_table* table = build_page_table(argv, &depth);

    //log bitmasks
    log_bitmasks(depth, table->bitmask);

    //create trace file and trace struct
    FILE *trace_file = fopen(filename, "r");
    p2AddrTr trace = {0};

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