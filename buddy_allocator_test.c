#include "buddy_allocator.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 260
#define BUDDY_LEVELS 5
#define MEMORY_SIZE 128
#define MIN_BUCKET_SIZE (BUFFER_SIZE>>(BUDDY_LEVELS))

#define UTIME (uint64_t)time(NULL)

char memory[MEMORY_SIZE]; // for the bitmap

BuddyAllocator alloc;

char date[32];

char* getDate(void) {
  time_t t = time(NULL);                        
  struct tm tm = *localtime(&t);                
  sprintf(date,"%d-%02d-%02d %02d:%02d:%02d", 
          tm.tm_year + 1900,                    
          tm.tm_mon + 1,                        
          tm.tm_mday,                           
          tm.tm_hour,                           
          tm.tm_min,                            
          tm.tm_sec);
  return date;
}


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

    printf("%s|memory request\n", getDate());
    void *p1 = BuddyAllocator_malloc(&alloc, 4);
    printf("%s|memory address [ %p ]\n", getDate(), p1);

    return 0;
}