#include "TLB_table.h"

TLB_table *create_table(int capacity)
{
    TLB_table *tlb = (TLB_table *)malloc(sizeof(TLB_table));
    tlb->table = (TLB_entry *)malloc(capacity * sizeof(TLB_entry));
    tlb->size = 0;
    tlb->capacity = capacity;
    return tlb;
}
void add_to_table(TLB_table *tlb, int addr, int frame)
{
    tlb->table = (TLB_entry *)realloc(tlb->table, tlb->size * sizeof(TLB_entry));
    tlb->table[tlb->size].address = addr;
    tlb->table[tlb->size].frame_number = frame;
    tlb->size++;
}
void delete_from_table(TLB_table *tlb, int address)
{
    int indexToDel;
    for (int i = 0; i < tlb->size; i++)
    {
        if (tlb->table[i].address == address)
        {
            indexToDel = i;
            break;
        }
    }
    for (int i = indexToDel; i < tlb->size - 1; i++)
    {
        tlb->table[i] = tlb->table[i + 1];
    }
    tlb->size--;
}
int get_frame_number(TLB_table *tlb, int address)
{
    for (int i = 0; i < tlb->size; i++)
    {
        if (tlb->table[i].address == address)
        {
            return tlb->table[i].frame_number;
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
        printf("address: %d, frame: %d\n", tlb->table[i].address, tlb->table[i].frame_number);
    }
}