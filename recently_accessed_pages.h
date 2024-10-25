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

typedef struct recently_accessed_pages_table
{
    address_time_pair **table;
    int size;
    int capacity;
} recently_accessed_pages_table;

recently_accessed_pages_table *create_table();
bool is_full(recently_accessed_pages_table *tbl);
void update_time_accessed(recently_accessed_pages_table *tbl, int address, int time_accessed);
void add_to_recent(recently_accessed_pages_table *tbl, uint32_t addr, int time_accessed);
int get_index_of_least_recently_accessed(recently_accessed_pages_table *tbl);
void remove_oldest(recently_accessed_pages_table *tbl);
int get_time_accessed(recently_accessed_pages_table *tbl, int address);
void print_recently_accessed_pgs(recently_accessed_pages_table *tbl);

#endif