#include "TLB_table.h"

/**
 * Halie Do
 * 827707836
 */

/**
 * @brief - create and initialize TLB (Translation lookalike buffer) table
 *
 * @param capacity - used to allocate size of the tlb table
 * @return - initialized tlb table
 */
TLB_table *create_tlb_table(int capacity)
{
    TLB_table *tlb = (TLB_table *)malloc(sizeof(TLB_table));
    tlb->table = (TLB_entry **)malloc(capacity * sizeof(TLB_entry *));
    tlb->size = 0;
    tlb->capacity = capacity;
    return tlb;
}

/**
 * @brief - add an entry to tlb table
 *
 * @param - virtual page number to be added
 * @param - frame number associated with the virtual page number
 */
void add_to_table(TLB_table *tlb, uint32_t addr, int frame)
{
    if (table_full(tlb))
    {
        return;
    }
    TLB_entry *new_entry = (TLB_entry *)malloc(sizeof(TLB_entry));
    new_entry->address = addr;
    new_entry->frame_number = frame;
    tlb->table[tlb->size] = new_entry;
    tlb->size++;
}

/**
 * @brief - delete an entry with the target address from the tlb table
 *
 * @param tlb - pointer to tlb table struct
 * @param address - address to remove from table
 */
void delete_from_table(TLB_table *tlb, uint32_t address)
{
    int shiftStart;
    // find the address in the table
    for (int i = 0; i < tlb->size; i++)
    {
        if (tlb->table[i] == NULL)
        {
            printf("NULL entry\n");
            continue;
        }
        if (tlb->table[i]->address == address)
        {
            free(tlb->table[i]);
            tlb->table[i] = NULL;
            shiftStart = i;
            break;
        }
    }
    // shift all entries to the left
    for (int i = shiftStart; i < tlb->size - 1; i++)
    {
        tlb->table[i] = tlb->table[i + 1];
    }
    // set the last entry to NULL
    tlb->table[tlb->size] = NULL;
    tlb->size--;
}

/**
 * @brief - return frame number associated with given virtual page number
 *
 * @param - pointer to tlb table
 * @param address - address to look up
 */
int get_frame_number(TLB_table *tlb, uint32_t address)
{
    for (int i = 0; i < tlb->size; i++)
    {
        if (tlb->table[i]->address == address)
        { // address found
            return tlb->table[i]->frame_number;
        }
    }
    return -1; // no frame number found, meaning address wasn't in the table
}

/**
 * @brief - return true if tlb table is full, otherwise false
 *
 * @param tlb - pointer to tlb table
 */
bool table_full(TLB_table *tlb)
{
    return tlb->size == tlb->capacity;
}

/**
 * @brief - return address that was least recently accessed in tlb table
 *
 * @param tbl - pointer to recency table struct to look up
 * @param tlb - pointer to tlb table struct
 */
uint32_t get_least_recently_accessed_pg(recency_table *tbl, TLB_table *tlb)
{
    int last_accessed = get_time_accessed(tbl, tlb->table[0]->address); // holds time access of the least recently accessed page
    uint32_t least_recently_accessed_pg = tlb->table[0]->address;
    int curr_time_accessed; // hold time access of current page that was being iterated
    for (int i = 0; i < tlb->size; i++)
    {
        curr_time_accessed = get_time_accessed(tbl, tlb->table[i]->address);
        if (curr_time_accessed < last_accessed) // update time access and address of least recently accessed page if an older one if found
        {
            last_accessed = curr_time_accessed;
            least_recently_accessed_pg = tlb->table[i]->address;
        }
    }
    return least_recently_accessed_pg;
}
