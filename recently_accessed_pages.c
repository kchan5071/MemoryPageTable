#include "recently_accessed_pages.h"

recency_table *create_recency_table()
{
    recency_table *tbl = (recency_table *)malloc(sizeof(recency_table));
    tbl->table = (address_time_pair **)malloc(CAPACITY * sizeof(address_time_pair *));
    tbl->size = 0;
    tbl->capacity = CAPACITY;
    return tbl;
}

bool is_full(recency_table *tbl)
{
    return tbl->size == tbl->capacity;
}

bool update_time_accessed(recency_table *tbl, int address, int time_accessed)
{
    for (int i = 0; i < tbl->size; i++)
    {
        if (tbl->table[i]->address == address)
        {
            tbl->table[i]->time_accessed = time_accessed;
            return true;
        }
    }
    return false;
}

void add_to_recent(recency_table *tbl, uint32_t addr, int time_accessed)
{
    address_time_pair *new_entry = (address_time_pair *)malloc(sizeof(address_time_pair));
    new_entry->address = addr;
    new_entry->time_accessed = time_accessed;
    tbl->table[tbl->size] = new_entry;
    tbl->size++;
}

int get_address_of_least_recently_accessed(recency_table *tbl)
{
    int last_accessed = tbl->table[0]->time_accessed;
    int index = 0;
    for (int i = 1; i < tbl->size; i++)
    {
        if (tbl->table[i]->time_accessed < last_accessed)
        {
            last_accessed = tbl->table[i]->time_accessed;
            index = i;
        }
    }
    return tbl->table[index]->address;
}



int get_time_accessed(recency_table *tbl, int addr)
{
    for (int i = 0; i < tbl->size; i++)
    {
        if (tbl->table[i]->address == addr)
        {
            return tbl->table[i]->time_accessed;
        }
    }
    return -1;
}

void print_recently_accessed_pgs(recency_table *tbl)
{
    printf("Recently accessed page numbers: \n");
    for (int i = 0; i < tbl->size; i++)
    {
        printf("Page number: 0x%08X, time accessed: %d\n", tbl->table[i]->address, tbl->table[i]->time_accessed);
    }
}