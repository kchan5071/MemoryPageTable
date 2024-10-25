#ifndef TBL_table_H
#define TLB_table_H
#include <stdbool.h>

typedef struct TLB_entry
{
    int address;
    int frame_number;
} TLB_entry;

typedef struct TLB_table
{
    TLB_entry *table;
    int size;
    int capacity;
} TLB_table;

TLB_table *create_table(int capacity);
void add_to_table(TLB_table *tlb, int addr, int frame);
void delete_from_table(TLB_table *tlb, int addr);
int get_frame_number(TLB_table *tlb, int addr);
bool table_full(TLB_table *tlb);

#endif