#include "TLB_table.h"

TLB_table *create_table(int capacity)
{
    TLB_table *tlb = (TLB_table *)malloc(sizeof(TLB_table));
    tlb->table = (TLB_entry **)malloc(capacity * sizeof(TLB_entry *));
    tlb->size = 0;
    tlb->capacity = capacity;
    return tlb;
}
void add_to_table(TLB_table *tlb, int addr, int frame)
{
    tlb->table = (TLB_entry **)realloc(tlb->table, (tlb->size + 1) * sizeof(TLB_entry *));
    TLB_entry *new_entry = (TLB_entry *)malloc(sizeof(TLB_entry));
    new_entry->address = addr;
    new_entry->frame_number = frame;
    tlb->table[tlb->size] = new_entry;
    tlb->size++;
}
void delete_from_table(TLB_table *tlb, int address)
{
    int shiftStart;
    for (int i = 0; i < tlb->size; i++)
    {
        if (tlb->table[i]->address == address)
        {
            free(tlb->table[i]);
            tlb->table[i] = NULL;
            shiftStart = i;
            break;
        }
    }
    for (int i = shiftStart; i < tlb->size - 1; i++)
    {
        tlb->table[i] = tlb->table[i + 1];
    }
    tlb->table[tlb->size] = NULL;
    tlb->size--;
}
int get_frame_number(TLB_table *tlb, int address)
{
    for (int i = 0; i < tlb->size; i++)
    {
        if (tlb->table[i]->address == address)
        {
            return tlb->table[i]->frame_number;
        }
    }
    return -1;
}

bool table_full(TLB_table *tlb)
{
    return tlb->size == tlb->capacity;
}

void print_table(TLB_table *tlb)
{
    for (int i = 0; i < tlb->size; i++)
    {
        printf("address: %d, frame: %d\n", tlb->table[i]->address, tlb->table[i]->frame_number);
    }
}