#include "TLB_table.h"

TLB_table *create_tlb_table(int capacity)
{
    TLB_table *tlb = (TLB_table *)malloc(sizeof(TLB_table));
    tlb->table = (TLB_entry **)malloc(capacity * sizeof(TLB_entry *));
    tlb->size = 0;
    tlb->capacity = capacity;
    return tlb;
}
void add_to_table(TLB_table *tlb, uint32_t addr, int frame) {
    TLB_entry *new_entry = (TLB_entry *)malloc(sizeof(TLB_entry));
    new_entry->address = addr;
    new_entry->frame_number = frame;
    // printf("Inserted--> address: 0x%08X, frame: %d\n", new_entry->address, new_entry->frame_number);
    tlb->table[tlb->size] = new_entry;
    tlb->size++;
}

void delete_from_table(TLB_table *tlb, uint32_t address) {
    int shiftStart;
    for (int i = 0; i < tlb->size; i++) {
        if (tlb->table[i] == NULL) {
            printf("NULL entry\n");
            continue;
        }
        if (tlb->table[i]->address == address) {
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
    // printf("Deleted successfully\n");
    tlb->size--;
}

int get_frame_number(TLB_table *tlb, uint32_t address) {
    for (int i = 0; i < tlb->size; i++) {
        if (tlb->table[i]->address == address) {
            return tlb->table[i]->frame_number;
        }
    }
    return -1;
}

bool table_full(TLB_table *tlb) {
    return tlb->size == tlb->capacity;
}

void print_table(TLB_table *tlb) {
    for (int i = 0; i < tlb->size; i++) {
        printf("address: 0x%08X, frame: %d\n", tlb->table[i]->address, tlb->table[i]->frame_number);
    }
}