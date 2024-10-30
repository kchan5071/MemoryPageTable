#include "recently_accessed_pages.h"

/**
 * Halie Do
 * 827707836
 */

/**
 * @brief - create and initialize table for keeping track of 10 most recently accessed virtual page numbers
 */
recency_table *create_recency_table()
{
    recency_table *tbl = (recency_table *)malloc(sizeof(recency_table));
    tbl->table = (address_time_pair **)malloc(CAPACITY * sizeof(address_time_pair *));
    tbl->size = 0;
    tbl->capacity = CAPACITY; // by default capacity is 10
    return tbl;
}

/**
 * @brief - update the time accessed of the given address in the table
 *
 * @param tbl - pointer to the recency table struct
 * @param address - address to update in the table
 * @param time_accessed - most recent time the address was accessed
 */
bool update_time_accessed(recency_table *tbl, uint32_t address, int time_accessed)
{
    for (int i = 0; i < tbl->size; i++)
    {
        if (tbl->table[i]->address == address) // address found
        {
            tbl->table[i]->time_accessed = time_accessed; // update time access
            return true;
        }
    }
    return false;
}

/**
 * @brief - Add a new entry into recency table (new address with the time it was most recently accessed)
 *
 * @param tbl - pointer to struct recency table where the update will be made
 * @param addr - Address to be added to the table
 * @param time_accessed - time accessed to be paired with the newly inserted address
 */
void add_to_recent(recency_table *tbl, uint32_t addr, int time_accessed)
{
    if (tbl->size == tbl->capacity) // if table is full, remove the entry with least recently accessed page
    {
        remove_oldest(tbl);
    }
    address_time_pair *new_entry = (address_time_pair *)malloc(sizeof(address_time_pair));
    new_entry->address = addr;
    new_entry->time_accessed = time_accessed;
    tbl->table[tbl->size] = new_entry; // append new page entry into recency table
    tbl->size++;                       // increment number of elements in table
}

/**
 * @brief - remove the least recently accessed address from the table
 *
 * @param tbl - pointer to struct recency table to update
 */
void remove_oldest(recency_table *tbl)
{
    int index_to_remove = get_index_of_least_recently_accessed(tbl);
    free(tbl->table[index_to_remove]); // free pointer to invalid address
    // shift the rest of the elements to the left
    for (int i = index_to_remove; i < tbl->size - 1; i++)
    {
        tbl->table[i] = tbl->table[i + 1];
    }
    tbl->table[tbl->size] = NULL;
    tbl->size--;
}

/**
 * @brief - return index of the entry with address least recently accessed
 *
 * @param tbl - pointer to recency table struct to do the look up
 */
int get_index_of_least_recently_accessed(recency_table *tbl)
{
    int index_to_remove = 0;
    for (int i = 0; i < tbl->size; i++)
    { // find index of entry with least recently accessed page
        if (tbl->table[i]->time_accessed < tbl->table[index_to_remove]->time_accessed)
        {
            index_to_remove = i;
        }
    }
    return index_to_remove;
}

/**
 * @brief - return time accessed of an address in recency table
 *
 * @param tbl - pointer to recency table struct
 * @param addr - address to look up
 */
int get_time_accessed(recency_table *tbl, uint32_t addr)
{
    for (int i = 0; i < tbl->size; i++)
    {
        if (tbl->table[i]->address == addr) // found target page
        {
            return tbl->table[i]->time_accessed; // return time it was most recently accessed
        }
    }
    return -1; // return -1 if page not found
}
