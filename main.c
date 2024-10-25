#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <getopt.h>
#include "tracereader.h"
#include "log.h"
#include "page_table.h"
#include "TLB_table.h"

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
    // default is "none"
    char *output_mode;
    int number_of_args;
} args;

static struct args *parse_opt(int argc, char **argv)
{
    struct args *args = malloc(sizeof(struct args));
    // default values
    args->cache_capacity = 0;
    args->number_of_addresses = -1;
    args->output_mode = "none";
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

int main(int argc, char **argv)
{

    // read args
    int depth = 0;

    // parse arguments
    struct args *args = parse_opt(argc, argv);

    // DELETE LATER
    //  printf("cache_capacity: %d\n", args->cache_capacity);
    //  printf("number_of_addresses: %d\n", args->number_of_addresses);
    //  printf("output_mode: %s\n", args->output_mode);

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

    // DELETE LATER
    //  for (int i = 0; i < depth; i++) {
    //      printf("depth_array[%d]: %d\n", i, depth_array[i]);
    //  }

    // check for validity
    check_for_validity(depth_array, depth);

    // create page table
    page_table *page_table = build_page_table(argv, &depth, depth_array);

    // create TLB table
    TLB_table *tlb = create_table(args->cache_capacity);

    // create trace file and trace struct
    p2AddrTr trace = {0};

    // loop through all addresses
    int address = -1;

    long iteration = 0;
    long hits = 0;
    long max = 0;

    while (NextAddress(trace_file, &trace))
    {
        uint32_t *indices = get_page_indices(trace.addr, page_table->bitmask, page_table->shift, depth);
        address_time_pair pair = record_page_access(page_table, page_table->root, indices, 0, depth, iteration);
        // check if the address is in the TLB
        int frame = get_frame_number(tlb, trace.addr);
        if (frame == -1)
        {
            // add to TLB
            add_to_table(tlb, trace.addr, pair.address);
        }
        if (pair.time_accessed != iteration)
        {
            hits++;
        }
        else
        {
            max = iteration;
        }
        // log accesses
        iteration++;
    }

    // printf("hits: %ld\n", hits);
    // printf("iteration: %ld\n", iteration);
    float hit_percent = (float)hits / (float)iteration;
    float miss_percent = 1 - hit_percent;
    // printf("hit_percent: %f\n", hit_percent);
    // printf("miss_percent: %f\n", miss_percent);
    print_table(tlb);
    // log_summary(4096, hits, iteration - hits, iteration, 0, max);
    fclose(trace_file);
    return 0;
}