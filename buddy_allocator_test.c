#include "buddy_allocator.h"
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 512
#define BUDDY_LEVELS 6
#define MEMORY_SIZE 128
#define MIN_BUCKET_SIZE (BUFFER_SIZE>>(BUDDY_LEVELS))

char memory[MEMORY_SIZE]; // for the bitmap

BuddyAllocator alloc;

int main(int argc, char** argv) {

    int buf_size = BUFFER_SIZE;
    int levels = BUDDY_LEVELS;    
    int min_bucket_size = buf_size >> (levels);

    char buffer[buf_size]; 

    // we initialize the allocator
    int construct = BuddyAllocator_init(&alloc, 
        levels, //numero di livelli
        buffer, // buffer per l'allocator
        buf_size, //dimensione del buffer per il buddy alloc
        memory,  // buffer per la bitmap
        MEMORY_SIZE,  //memoria per la bitmap
        min_bucket_size);  
    if (construct==0){
        //printf("Qualcosa è andato storto nell'inizializzazione del Buddy Allocator");
        return 0;
    }
    return 0;
}