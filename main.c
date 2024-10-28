#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>
#include "tracereader.h"
#include "log.h"
#include "page_table.h"
#include "TLB_table.h"
#include "recently_accessed_pages.h"

#define ADDRESS_LENGTH 32

/**
 * Kai Chan
 * 827673009
 *
 * Halie Do
 * 827707836
 */

typedef struct args
{
    // cache capcaity, 0 by default
    int cache_capacity;
    // does all addresses by default, which  is encoded as -1
    int number_of_addresses;
    // default is "summary"
    char *output_mode;
    int number_of_args;
} args;

static struct args *parse_opt(int argc, char **argv)
{
    struct args *args = malloc(sizeof(struct args));
    // default values
    args->cache_capacity = 0;
    args->number_of_addresses = -1;
    args->output_mode = "summary";
    args->number_of_args = 0;
    // parse options
    int opt;
    while ((opt = getopt(argc, argv, "c:n:o:")) != -1)
    {
        switch (opt)
        {
        case 'c':
            args->cache_capacity = atoi(optarg);
            args->number_of_args += 2;
            break;
        case 'n':
            args->number_of_addresses = atoi(optarg);
            args->number_of_args += 2;
            break;
        case 'o':
            args->output_mode = optarg;
            args->number_of_args += 2;
            break;
        default:
            fprintf(stderr, "Usage: %s [-c cache capacity] [-n number of addresses] [-o output mode] filename\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    return args;
}

static char *get_filename(int argc, char **argv)
{
    // search args for .tr suffix
    for (int i = 1; i < argc; i++)
    {
        if (strstr(argv[i], ".tr") != NULL)
        {
            return argv[i];
        }
    }
    printf("Unable to open <<%s>>\n", argv[1]);
    return NULL;
}

static uint32_t *get_depth(int number_of_bits, int argc, char **argv)
{
    // take in the rest of the arguments and put them into an int array
    uint32_t *depth_array = malloc(sizeof(uint32_t) * number_of_bits);
    // iterate through the number of bits and convert them to ints
    for (int i = 0; i < number_of_bits; i++)
    {
        depth_array[i] = atoi(argv[i + (argc - number_of_bits)]);
    }
    return depth_array;
}

static void check_for_validity(uint32_t *depth_array, int number_of_bits)
{
    // check to see that the number of bits is greater than 0
    // and the total is less than 28
    int total = 0;
    for (int i = 0; i < number_of_bits; i++)
    {
        total += depth_array[i];
        if (depth_array[i] <= 0)
        {
            printf("Level 0 page table must be at least 1 bit\n");
        }
    }
    if (total > 28)
    {
        printf("Too many bits used in page tables\n");
    }
}

static uint32_t get_virtual_page_number(uint32_t addr, uint32_t *num_of_mask_bits_arr, int num_of_levels) {
    uint32_t bits_to_mask = 0;
    for (int i = 0; i < num_of_levels; i++)
    {
        bits_to_mask += num_of_mask_bits_arr[i];
    }
    unsigned int mask = 1;
    for (int b = 1; b < bits_to_mask; b++)
    {
        mask = mask << 1;
        mask = mask | 1;
    }
    mask = mask << (ADDRESS_LENGTH - bits_to_mask);
    return (addr & mask);
}

static uint32_t get_virtual_address(uint32_t virtual_page_number, uint32_t offset, int offset_size) {
    return (virtual_page_number << offset_size) + offset;
}

static uint32_t get_offset(int addr, int offset_size) {
    uint32_t mask = 1;
    for (int b = 1; b < offset_size; b++)
    {
        mask = mask << 1;
        mask = mask | 1;
    }
    return addr & mask;
}

static uint32_t get_offset_size(uint32_t* depth_array, int depth) {
    int offset_size = 32;
    for (int i = 0; i < depth; i++) {
        offset_size -= depth_array[i];
    }
    return offset_size;
}

int main(int argc, char **argv) {
    char modes[5][20] = {"bitmasks", "va2pa", "va2pa_atc_ptwalk", "vpn2pfn", "offset"};

    // read args
    int depth = 0;

    // parse arguments
    struct args *args = parse_opt(argc, argv);

    // DELETE LATER
    //  printf("cache_capacity: %d\n", args->cache_capacity);
    //  printf("number_of_addresses: %d\n", args->number_of_addresses);
    //  printf("output_mode:%s\n", args->output_mode);
    //  printf("mode 5:%s\n", modes[4]);
    //  bool matches = strcmp(args->output_mode, modes[4]) == 0;
    //  printf("offset matches: %d\n", matches);

    // check for filename
    char *filename = get_filename(argc, argv);
    FILE *trace_file = fopen(filename, "r");
    if (trace_file == NULL || filename == NULL)
    {
        printf("Unable to open <<%s>>\n", filename);
        exit(0);
    }
    else
    {
        args->number_of_args++;
    }

    // read the rest of the arguments into an int array
    depth = argc - args->number_of_args - 1;
    uint32_t *depth_array = get_depth(depth, argc, argv);
    // for (int i = 0; i < depth; i++)
    // {
    //     printf("depth_array[%d]: %d\n", i, depth_array[i]);
    // }

    // check for validity
    check_for_validity(depth_array, depth);

    // create page table
    page_table *page_table = build_page_table(argv, &depth, depth_array);

    // create TLB table
    TLB_table *tlb = create_tlb_table(args->cache_capacity);

    recency_table *recent_pages_tbl = create_recency_table();

    // create trace file and trace struct
    p2AddrTr trace = {0};

    //get size of offset
    int offset_size = get_offset_size(depth_array, depth);

    // loop through all addresses
    long iteration = 0;
    long cache_hits = 0;
    long page_table_hits = 0;
    long max = 0;
    int frame_number = 0;

    while (NextAddress(trace_file, &trace)) {
        //check if there are addresses or if we have read all the addresses
        if (args->number_of_addresses != -1 && iteration == args->number_of_addresses) {
            break;
        }
        
        uint32_t virtual_pg_num = get_virtual_page_number(trace.addr, depth_array, depth);
        uint32_t *indices = get_page_indices(trace.addr, page_table->bitmask, page_table->shift, depth);
        map page_info = record_page_access(page_table, page_table->root, indices, 0, depth, iteration, &frame_number, virtual_pg_num);
        uint32_t offset = get_offset(trace.addr, offset_size);
        uint32_t virtual_address = get_virtual_address(page_info.frame_number, offset, offset_size);

        // check if the address is in the recently accessed pages
        bool recently_accessed = false;
        int time_accessed = get_time_accessed(recent_pages_tbl, page_info.address);
        if (time_accessed == -1) {
            add_to_recent(recent_pages_tbl, page_info.address, page_info.time_accessed);
        }
        else {
            update_time_accessed(recent_pages_tbl, page_info.address, page_info.time_accessed);
            recently_accessed = true;
            page_table_hits++;
        }
        
        // check if the address is in the TLB
        bool found_in_tlb = false;
        if (args->cache_capacity != 0) {
            found_in_tlb = get_frame_number(tlb, virtual_address) != -1;
            if (!found_in_tlb && !table_full(tlb)) {      // not in TLB and TLB not full
                // add to TLB
                add_to_table(tlb, virtual_pg_num, page_info.frame_number);
            }
            else if (!found_in_tlb && (tlb)) {  // not in TLB and TLB full
                // get least recently accessed index
                int address_to_remove = get_address_of_least_recently_accessed(recent_pages_tbl);
                // delete from TLB
                delete_from_table(tlb, address_to_remove);
                // add to TLB
                add_to_table(tlb, virtual_pg_num, page_info.frame_number);
            }
            else if (found_in_tlb) {    // in TLB
                cache_hits++;
            }
        }

        // log accesses
        if (strcmp(args->output_mode, modes[1]) == 0) { // va2pa
            log_virtualAddr2physicalAddr(trace.addr, virtual_address);
        }
        else if (strcmp(args->output_mode, modes[2]) == 0) { // va2pa_atc_ptwalk
            log_va2pa_ATC_PTwalk(trace.addr, virtual_address, recently_accessed, found_in_tlb);
        }
        else if (strcmp(args->output_mode, modes[3]) == 0) { // vpn2pfn
            log_pagemapping(depth, indices, page_info.frame_number);
        }
        else if (strcmp(args->output_mode, modes[4]) == 0) { // offset
            hexnum(offset);
        }

        iteration++;
    }
    if (strcmp(args->output_mode, modes[0]) == 0) { // bitmasks
        log_bitmasks(depth, page_table->bitmask);
    }
    if (strcmp(args->output_mode, modes[4]) != 0) {
        log_summary(pow(2, offset_size), 
                    cache_hits, page_table_hits, 
                    iteration, 
                    frame_number, 
                    recent_pages_tbl->size);
    }
    return 0;
}