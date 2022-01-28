#pragma once
#include "bitmap.h"

#define MAX_LEVELS 16

typedef struct  {
  BitMap bitmap; //bitmap associata al Buddy Allocator
  int num_levels;
  char* buffer; // the buffer area to be managed
  int buffer_size;
  int min_bucket_size; // the minimum page of RAM that can be returned
} BuddyAllocator;

// initializes the buddy allocator, and checks that the buffer is large enough
int BuddyAllocator_init(BuddyAllocator* alloc,
                         int num_levels,
                         char* alloc_buf, // buffer per l'allocator
                         int alloc_buf_size,
                         char* bitmap_buf, // buffer per la bitmap
                         int bitmap_buf_size,
                         int min_bucket_size);

//allocates memory
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size);

//releases allocated memory
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem);

void Set_status_parents(BitMap *bit_map, int bit_num, int status);

void Set_status_children(BitMap *bit_map, int bit_num, int status);

void merge(BitMap *bitmap, int idx);