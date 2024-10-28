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
    if (table_full(tlb)) {
        return;
    }
    TLB_entry *new_entry = (TLB_entry *)malloc(sizeof(TLB_entry));
    new_entry->address = addr;
    new_entry->frame_number = frame;
    tlb->table[tlb->size] = new_entry;
    tlb->size++;
}

int get_least_recent_index(TLB_table *tlb) {
    int index_to_remove = 0;
    for (int i = 0; i < tlb->size; i++) {
        if (tlb->table[i]->frame_number > tlb->table[index_to_remove]->frame_number) {
            index_to_remove = i;
        }
    }
    return index_to_remove;
}

void delete_index_from_table(TLB_table *tlb, int index) {
    free(tlb->table[index]);
    for (int i = index; i < tlb->size - 1; i++) {
        tlb->table[i] = tlb->table[i + 1];
    }
    tlb->table[tlb->size] = NULL;
    tlb->size--;
}

void delete_from_table(TLB_table *tlb, uint32_t address) {
    int shiftStart;
    //find the address in the table
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
    //shift all entries to the left
    for (int i = shiftStart; i < tlb->size - 1; i++)
    {
        tlb->table[i] = tlb->table[i + 1];
    }
    //set the last entry to NULL
    tlb->table[tlb->size] = NULL;
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