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
    tbl->capacity = CAPACITY;
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
        if (tbl->table[i]->address == address)
        {
            tbl->table[i]->time_accessed = time_accessed;
            return true;
        }
    }
    return false;
}

/**
 * @brief - Add a new entry (new address with the time it was most recently accessed)
 *
 * @param addr - Address to be added to the table
 * @param time_accessed - time accessed to be paired with the newly inserted address
 */
void add_to_recent(recency_table *tbl, uint32_t addr, int time_accessed)
{
    if (tbl->size == tbl->capacity)
    {
        remove_oldest(tbl);
    }
    address_time_pair *new_entry = (address_time_pair *)malloc(sizeof(address_time_pair));
    new_entry->address = addr;
    new_entry->time_accessed = time_accessed;
    tbl->table[tbl->size] = new_entry;
    tbl->size++;
}

/**
 * @brief - remove the least recently accessed address from the table
 *
 * @param tbl - pointer to struct recency table to update
 */
void remove_oldest(recency_table *tbl)
{
    int index_to_remove = get_index_of_least_recently_accessed(tbl);
    free(tbl->table[index_to_remove]);
    for (int i = index_to_remove; i < tbl->size - 1; i++)
    {
        tbl->table[i] = tbl->table[i + 1];
    }
    tbl->table[tbl->size] = NULL;
    tbl->size--;
}

/**
 * @brief - return address that was least recently accessed
 *
 * @param - pointer to recency table struct to look up
 */
uint32_t get_address_of_least_recently_accessed(recency_table *tbl, TLB_table *tlb)
{
    int last_accessed = get_time_accessed(tbl, tlb->table[0]->address);
    uint32_t least_recently_accessed_pg = tlb->table[0]->address;
    int curr_time_accessed;
    for (int i = 0; i < tlb->size; i++)
    {
        curr_time_accessed = get_time_accessed(tbl, tlb->table[i]->address);
        if (curr_time_accessed < last_accessed)
        {
            last_accessed = curr_time_accessed;
            least_recently_accessed_pg = tlb->table[i]->address;
        }
    }
    return least_recently_accessed_pg;
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
    {
        if (tbl->table[i]->time_accessed > tbl->table[index_to_remove]->time_accessed)
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
        if (tbl->table[i]->address == addr)
        {
            return tbl->table[i]->time_accessed;
        }
    }
    return -1;
}
