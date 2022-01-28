#include "buddy_allocator.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define BUFFER_SIZE 256
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

/* Signal Handler for SIGINT */
void sigintHandler(int sig_num){
    printf("\nSIGINT or CTRL-C detected. Exiting gracefully\n");
    fflush(stdout);
    exit(0);
}


int main(int argc, char** argv) {

    signal(SIGINT, sigintHandler);

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


    sleep(2);

    printf("\n<----------------------------------TEST FULL ALLOC------------------------------------->\n\n");

    sleep(2);

    printf("\n\033[35m%s|memory request\033[0m\n", getDate());
    void *p1 = BuddyAllocator_malloc(&alloc, 123);
    printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p1);

    
        
    printf("\n<---------------------------------------------------------------------------------------->\n");
    sleep(3);

    printf("\n\033[35m%s|memory request\033[0m\n", getDate());
    void *p2 = BuddyAllocator_malloc(&alloc, 123);
    printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p2);


    printf("\n<---------------------------------------------------------------------------------------->\n");
    sleep(3);


    printf("\n\n\033[32m%s|memory release\033[0m\n", getDate());
    BuddyAllocator_free(&alloc, p1);

    printf("\n<---------------------------------------------------------------------------------------->\n");
    sleep(3);

    printf("\n\n\033[32m%s|memory release\033[0m\n", getDate());
    BuddyAllocator_free(&alloc, p2);

    printf("\n<---------------------------------------------------------------------------------------->\n");
    sleep(3);


    printf("\n\033[35m%s|memory request\033[0m\n", getDate());
    void *p3 = BuddyAllocator_malloc(&alloc, 252);
    printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p3);

    printf("\n<---------------------------------------------------------------------------------------->\n");
    sleep(3);

    printf("\n\n\033[32m%s|memory release\033[0m\n", getDate());
    BuddyAllocator_free(&alloc, p3);

    printf("\n<---------------------------------------------------------------------------------------->\n");

    return 0;
}
