#include "recently_accessed_pages.h"

recently_accessed_pages_table *create_table()
{
    recently_accessed_pages_table *tbl = (recently_accessed_pages_table *)malloc(sizeof(recently_accessed_pages_table));
    tbl->table = (address_time_pair **)malloc(CAPACITY * sizeof(address_time_pair *));
    tbl->size = 0;
    tbl->capacity = CAPACITY;
    return tbl;
}

bool is_full(recently_accessed_pages_table *tbl)
{
    return tbl->size == tbl->capacity;
}

void update_time_accessed(recently_accessed_pages_table *tbl, int address, int time_accessed)
{
    for (int i = 0; i < tbl->size; i++)
    {
        if (tbl->table[i]->address == address)
        {
            tbl->table[i]->time_accessed = time_accessed;
            break;
        }
    }
}

void add_to_recent(recently_accessed_pages_table *tbl, address_time_pair *pair)
{
    if (tbl->size == tbl->capacity)
    {
        remove_oldest(tbl);
    }
    tbl->table = (address_time_pair **)realloc(tbl->table, (tbl->size + 1) * sizeof(address_time_pair *));
    tbl->table[tbl->size] = pair;
    tbl->size++;
}
int get_index_of_least_recently_accessed(recently_accessed_pages_table *tbl)
{
    int index_to_remove = 0;
    for (int i = 0; i < tbl->size; i++)
    {
        if (tbl->table[i]->time_accessed > tbl->table[index_to_remove]->time_accessed)
        {
            index_to_remove = i;
        }
    }
    return index_to_remove;
}
void remove_oldest(recently_accessed_pages_table *tbl)
{
    int index_to_remove = get_index_of_least_recently_accessed(tbl);
    free(tbl->table[index_to_remove]);
    tbl->table[index_to_remove] = NULL;
    for (int i = index_to_remove; i < tbl->size - 1; i++)
    {
        tbl->table[i] = tbl->table[i + 1];
    }
    tbl->table[tbl->size] = NULL;
    tbl->size--;
}

int get_time_accessed(recently_accessed_pages_table *tbl, int addr)
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