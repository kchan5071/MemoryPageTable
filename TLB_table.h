#ifndef TLB_TABLE_H
#define TLB_TABLE_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

typedef struct TLB_entry
{
    uint32_t address;
    int frame_number;
} TLB_entry;

typedef struct TLB_table
{
    TLB_entry **table;
    int size;
    int capacity;
} TLB_table;

TLB_table *create_tlb_table(int capacity);
void add_to_table(TLB_table *tlb, uint32_t addr, int frame);
void delete_from_table(TLB_table *tlb, uint32_t addr);
int get_frame_number(TLB_table *tlb, uint32_t addr);
bool table_full(TLB_table *tlb);

void print_table(TLB_table *tlb);

#endif