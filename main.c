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
            if (atoi(optarg) < 0)
            {
                fprintf(stderr, "Cache capacity must be a number, greater than or equal to 0.\n");
                exit(EXIT_FAILURE);
            }
            args->cache_capacity = atoi(optarg);
            args->number_of_args += 2;
            break;
        case 'n':
            if (atoi(optarg) < 0)
            {
                fprintf(stderr, "Number of memory accesses must be a number, greater than 0.\n");
                exit(EXIT_FAILURE);
            }
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

static uint32_t get_virtual_page_number(uint32_t addr, uint32_t *num_of_mask_bits_arr, int num_of_levels)
{
    uint32_t bits_to_mask = 0;
    for (int i = 0; i < num_of_levels; i++)
    { // add number of mask bits in each level to get total # of bits to mask
        bits_to_mask += num_of_mask_bits_arr[i];
    }
    // create mask
    unsigned int mask = 1;
    for (int b = 1; b < bits_to_mask; b++)
    {
        mask = mask << 1;
        mask = mask | 1;
    }
    mask = mask << (ADDRESS_LENGTH - bits_to_mask); // mask the # of bits needed to be masked at the beginning of the address
    return (addr & mask);                           // return virtual page number (masked address)
}

static uint32_t get_virtual_address(uint32_t virtual_page_number, uint32_t offset, int offset_size)
{
    return (virtual_page_number << offset_size) + offset;
}

static uint32_t get_offset(int addr, int offset_size)
{
    uint32_t mask = 1;
    for (int b = 1; b < offset_size; b++)
    {
        mask = mask << 1;
        mask = mask | 1;
    }
    return addr & mask;
}

static uint32_t get_offset_size(uint32_t *depth_array, int depth)
{
    int offset_size = 32;
    for (int i = 0; i < depth; i++)
    {
        offset_size -= depth_array[i];
    }
    return offset_size;
}

int main(int argc, char **argv)
{
    char modes[5][20] = {"bitmasks", "va2pa", "va2pa_atc_ptwalk", "vpn2pfn", "offset"};

    // read args
    int depth = 0;

    // parse arguments
    struct args *args = parse_opt(argc, argv);

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

    // check for validity
    check_for_validity(depth_array, depth);

    // create page table
    page_table *page_table = build_page_table(argv, &depth, depth_array);

    // create TLB table
    TLB_table *tlb = create_tlb_table(args->cache_capacity);

    recency_table *recent_pages_tbl = create_recency_table();

    // create trace file and trace struct
    p2AddrTr trace = {0};

    // get size of offset
    int offset_size = get_offset_size(depth_array, depth);

    // loop through all addresses
    long iteration = 0;
    long cache_hits = 0;
    long page_table_hits = 0;
    int frame_number = 0;
    bool finish_logging = false;
    map *page_info = NULL;

    while (NextAddress(trace_file, &trace))
    {
        bool pg_hit = false;
        bool tlb_hit = false;
        // check if there are addresses or if we have read all the addresses
        if (args->number_of_addresses != -1 && iteration == args->number_of_addresses)
        {
            break;
        }

        uint32_t virtual_pg_num = get_virtual_page_number(trace.addr, depth_array, depth);
        uint32_t *indices = get_page_indices(trace.addr, page_table->bitmask, page_table->shift, depth);
        uint32_t offset = get_offset(trace.addr, offset_size);

        if (args->cache_capacity != 0) // case: user specified cache capacity, TLB implemented
        {
            // check if the address is in the recently accessed pages
            int time_accessed = get_time_accessed(recent_pages_tbl, virtual_pg_num);
            if (time_accessed == -1) // insert address if not found in recent table
            {
                add_to_recent(recent_pages_tbl, virtual_pg_num, iteration);
            }
            else // updated time accessed if time was found
            {
                update_time_accessed(recent_pages_tbl, virtual_pg_num, iteration);
            }
            // check if the address is in the TLB
            tlb_hit = get_frame_number(tlb, virtual_pg_num) != -1;
            if (!tlb_hit)
            { // not in TLB and TLB not full
              // add to TLB
                if (table_full(tlb))
                {
                    uint32_t address_to_remove = get_least_recently_accessed_pg(recent_pages_tbl, tlb);
                    // delete least recently accessed page from TLB based on records in recency table
                    delete_from_table(tlb, address_to_remove);
                }
                page_info = lookup_vpn2pfn(page_table, page_table->root, indices, 0, depth);
                if (page_info) // if page was found in page table -> increment page table hits
                {
                    pg_hit = true;
                    ++page_table_hits;
                }
                else // if page was not found, insert new entry
                {
                    insert_vpn2pfn(page_table, page_table->root, indices, 0, depth, iteration, &frame_number, virtual_pg_num);
                    page_info = lookup_vpn2pfn(page_table, page_table->root, indices, 0, depth);
                }
                add_to_table(tlb, virtual_pg_num, page_info->frame_number); // add new virtual page number & its frame number into tlb table
            }
            else // if virtual address was found in tlb table -> increment cache hits
            {
                ++cache_hits;
            }
        }
        else // case: no implementation of tlb or recency table because cache capacity was not specified
        {
            page_info = lookup_vpn2pfn(page_table, page_table->root, indices, 0, depth);
            if (page_info) // page found in table
            {
                ++page_table_hits;
                pg_hit = true;
            }
            else // page not found -> insert new entry in table
            {
                insert_vpn2pfn(page_table, page_table->root, indices, 0, depth, iteration, &frame_number, virtual_pg_num);
            }
        }

        page_info = lookup_vpn2pfn(page_table, page_table->root, indices, 0, depth);
        uint32_t virtual_address = get_virtual_address(page_info->frame_number, offset, offset_size);

        // log accesses
        if (strcmp(args->output_mode, modes[1]) == 0)
        { // va2pa
            log_virtualAddr2physicalAddr(trace.addr, virtual_address);
        }
        else if (strcmp(args->output_mode, modes[2]) == 0)
        { // va2pa_atc_ptwalk
            log_va2pa_ATC_PTwalk(trace.addr, virtual_address, tlb_hit, pg_hit);
        }
        else if (strcmp(args->output_mode, modes[3]) == 0)
        { // vpn2pfn
            log_pagemapping(depth, indices, page_info->frame_number);
        }
        else if (strcmp(args->output_mode, modes[4]) == 0)
        { // offset
            hexnum(offset);
        }
        else
        {
            finish_logging = true;
        }

        iteration++;
    }
    if (!finish_logging)
    {
        return 0;
    }
    if (strcmp(args->output_mode, modes[0]) == 0)
    { // bitmasks
        log_bitmasks(depth, page_table->bitmask);
    }
    else if (strcmp(args->output_mode, modes[4]) != 0)
    {
        log_summary(pow(2, offset_size),
                    cache_hits, page_table_hits,
                    iteration,
                    frame_number,
                    page_table->num_of_entries);
    }
    return 0;
}