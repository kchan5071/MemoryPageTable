#ifndef RECENTLY_ACCESSED_PAGES_H
#define RECENTLY_ACCESSED_PAGES_H
#include "page_table.h"
#include <stdbool.h>
#define CAPACITY 10

typedef struct address_time_pair
{
    uint32_t address;
    int time_accessed;
} address_time_pair;

typedef struct recency_table
{
    address_time_pair **table;
    int size;
    int capacity;
} recency_table;

recency_table *create_recency_table();
bool is_full(recency_table *tbl);
bool update_time_accessed(recency_table *tbl, int address, int time_accessed);
void add_to_recent(recency_table *tbl, uint32_t addr, int time_accessed);
int get_index_of_least_recently_accessed(recency_table *tbl);
void remove_oldest(recency_table *tbl);
int get_time_accessed(recency_table *tbl, int address);
void print_recently_accessed_pgs(recency_table *tbl);

#endif