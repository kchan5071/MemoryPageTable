#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "tracereader.h"
#include "log.h"
#include "page_table.h"

/**
 * Kai Chan 
 * 827673009
 */

struct args {
    //cache capcaity, 0 by default
    int cache_capacity;
    //does all addresses by default, which  is encoded as -1
    int number_of_addresses;
    //default is "none"
    char* output_mode;

};

static struct args* parse_opt(int argc, char** argv) {
    struct args* args = malloc(sizeof(struct args));
    args->cache_capacity = 0;
    args->number_of_addresses = -1;
    args->output_mode = "none";
    int opt;
    while ((opt = getopt(argc, argv, "c:n:o:")) != -1) {
        switch (opt) {
            case 'c':
                args->cache_capacity = atoi(optarg);
                break;
            case 'n':
                args->number_of_addresses = atoi(optarg);
                break;
            case 'o':
                args->output_mode = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s [-c cache_capacity] [-n number_of_addresses] [-o output_mode] filename\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    return args;
}

static char* get_filename(int argc, char** argv) {
    //search args for .tr suffix
    for (int i = 1; i < argc; i++) {
        if (strstr(argv[i], ".tr") != NULL) {
            return argv[i];
        }
    }
    return NULL;
}

int main(int argc, char** argv) {

    //read args
    int depth = 0;

    //parse arguments
    struct args* args = parse_opt(argc, argv);
    printf("cache_capacity: %d\n", args->cache_capacity);
    printf("number_of_addresses: %d\n", args->number_of_addresses);
    printf("output_mode: %s\n", args->output_mode);

    //check for filename
    char* filename = get_filename(argc, argv);
    printf("filename: %s\n", filename);


    //create page table
    page_table* table = build_page_table(argv, &depth);

    //log bitmasks
    log_bitmasks(depth, table->bitmask);

    //create trace file and trace struct
    FILE *trace_file = fopen(filename, "r");
    p2AddrTr trace = {0};

    //loop through all addresses
    int address = -1;
    while (NextAddress(trace_file, &trace)) {
        uint32_t* indices = get_page_indices(trace.addr, table->bitmask, table->shift, depth);
        int times_accessed = record_page_access(table, table->root, indices, 0, depth);
        //log accesses
        log_pgindices_numofaccesses(trace.addr, depth, indices, times_accessed);
    }
    fclose(trace_file);
    return 0;
}