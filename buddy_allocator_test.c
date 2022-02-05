#include "buddy_allocator.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#define BUFFER_SIZE 1025
#define BUDDY_LEVELS 5
#define MEMORY_SIZE 256

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

    int ris;

    if (argc > 1) {
        ris =  atoi(argv[1]);
        if (ris!= 0 && ris!= 1 && ris!= 2 && ris!= 3){
            printf("ERROR, usage: <%s> <x> dove x è:\n0 per eseguire tutti i test\n1 per eseguire test full alloc\n2 per eseguire test allocazione sparsa\n3 per eseguire test errori\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    } 
    else{
        printf("ERROR, usage: <%s> <x> dove x è:\n0 per eseguire tutti i test\n1 per eseguire test full alloc\n2 per eseguire test allocazione sparsa\n3 per eseguire test errori\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int buf_size = BUFFER_SIZE;
    int levels = BUDDY_LEVELS;    
    int min_bucket_size = buf_size >> (levels); // (buf_size) / (2^levels)

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
        printf("Qualcosa è andato storto nell'inizializzazione del Buddy Allocator");
        return 0;
    }

    if (ris == 1){

        printf("\n<----------------------------------TEST FULL ALLOC------------------------------------->\n\n");

        sleep(2);

        printf("\n\033[35m%s|memory request\033[0m\n", getDate());
        void *p1 = BuddyAllocator_malloc(&alloc, 125);
        printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p1);

    
        
        printf("\n<---------------------------------------------------------------------------------------->\n");
        sleep(3);

        printf("\n\033[35m%s|memory request\033[0m\n", getDate());
        void *p10 = BuddyAllocator_malloc(&alloc, 252);
        printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p10);

    
        
        printf("\n<---------------------------------------------------------------------------------------->\n");
        sleep(3);

        printf("\n\033[35m%s|memory request\033[0m\n", getDate());
        void *p2 = BuddyAllocator_malloc(&alloc, 508);
        printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p2);


        printf("\n<---------------------------------------------------------------------------------------->\n");
        sleep(3);


        printf("\n\033[32m%s|memory release\033[0m\n", getDate());
        BuddyAllocator_free(&alloc, p1);

        printf("\n<---------------------------------------------------------------------------------------->\n");
        sleep(3);

        printf("\n\033[32m%s|memory release\033[0m\n", getDate());
        BuddyAllocator_free(&alloc, p2);

        printf("\n<---------------------------------------------------------------------------------------->\n");
        sleep(3);

        printf("\n\033[32m%s|memory release\033[0m\n", getDate());
        BuddyAllocator_free(&alloc, p10);

        printf("\n<---------------------------------------------------------------------------------------->\n");
        sleep(3);


        printf("\n\033[35m%s|memory request\033[0m\n", getDate());
        void *p3 = BuddyAllocator_malloc(&alloc, 1020);
        printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p3);

        printf("\n<---------------------------------------------------------------------------------------->\n");
        sleep(3);

        printf("\n\033[32m%s|memory release\033[0m\n", getDate());
        BuddyAllocator_free(&alloc, p3);
        printf("\n<-------------------------------------FINE TEST 1----------------------------------------->\n\n");
    
        sleep(2);

        return 0;
    }

    else if (ris ==2){
        printf("\n<----------------------------------------TEST 2------------------------------------------->\n\n");

        sleep(2);

        printf("\n\033[35m%s|memory request\033[0m\n", getDate());
        void *p4 = BuddyAllocator_malloc(&alloc, 14);
        printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p4);

        printf("\n<---------------------------------------------------------------------------------------->\n");
        sleep(3);

        printf("\n\033[35m%s|memory request\033[0m\n", getDate());
        void *p5 = BuddyAllocator_malloc(&alloc, 28);
        printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p5);

        printf("\n<---------------------------------------------------------------------------------------->\n");
        sleep(3);

        printf("\n\033[35m%s|memory request\033[0m\n", getDate());
        void *p6 = BuddyAllocator_malloc(&alloc, 4);
        printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p6);

        printf("\n<---------------------------------------------------------------------------------------->\n");
        sleep(3);

        printf("\n\033[35m%s|memory request\033[0m\n", getDate());
        void *p7 = BuddyAllocator_malloc(&alloc, 5);
        printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p7);

        printf("\n<---------------------------------------------------------------------------------------->\n");
        sleep(3);

        printf("\n\033[35m%s|memory request\033[0m\n", getDate());
        void *p8 = BuddyAllocator_malloc(&alloc, 4);
        printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p8);

        printf("\n<---------------------------------------------------------------------------------------->\n");
        sleep(3);

        printf("\n\033[35m%s|memory request\033[0m\n", getDate());
        void *p11 = BuddyAllocator_malloc(&alloc, 18);
        printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p11);

        printf("\n<---------------------------------------------------------------------------------------->\n");
        sleep(3);

        printf("\n\033[32m%s|memory release\033[0m\n", getDate());
        BuddyAllocator_free(&alloc, p5);

        printf("\n<---------------------------------------------------------------------------------------->\n");
        sleep(3);

        printf("\n\033[32m%s|memory release\033[0m\n", getDate());
        BuddyAllocator_free(&alloc, p11);

        printf("\n<---------------------------------------------------------------------------------------->\n");
        sleep(3);

        printf("\n\033[32m%s|memory release\033[0m\n", getDate());
        BuddyAllocator_free(&alloc, p8);

        printf("\n<---------------------------------------------------------------------------------------->\n");
        sleep(3);

        printf("\n\033[32m%s|memory release\033[0m\n", getDate());
        BuddyAllocator_free(&alloc, p7);

        printf("\n<---------------------------------------------------------------------------------------->\n");
        sleep(3);

        printf("\n\033[32m%s|memory release\033[0m\n", getDate());
        BuddyAllocator_free(&alloc, p6);

        printf("\n<---------------------------------------------------------------------------------------->\n");
        sleep(3);

        printf("\n\033[32m%s|memory release\033[0m\n", getDate());
        BuddyAllocator_free(&alloc, p4);

        printf("\n<-------------------------------------FINE TEST 2----------------------------------------->\n\n");

        return 0;
    }
    else if (ris==3){
        
        
        printf("\n<-------------------------------------TEST 3 ERRORI--------------------------------------->\n\n");


        printf("ALLOCO UN BLOCCO MAGGIORE DELLA DIMENSIONE TOTALE\n"); 
    
        sleep(2);

        printf("\n\033[35m%s|memory request\033[0m\n", getDate());
        void *p12 = BuddyAllocator_malloc(&alloc, 10000);
        printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p12);

        printf("\n<---------------------------------------------------------------------------------------->\n");

    

        printf("RIEMPIO LA MEMORIA E PROVO AD ALLOCARE ALTRA MEMORIA\n");
    
        sleep(2);

        printf("\n\033[35m%s|memory request\033[0m\n", getDate());
        void *p13 = BuddyAllocator_malloc(&alloc, 1020);
        printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p13);

        printf("\n<---------------------------------------------------------------------------------------->\n");

        sleep(2);

        printf("\n\033[35m%s|memory request\033[0m\n", getDate());
        void *p14 = BuddyAllocator_malloc(&alloc, 1020);
        printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p14);

        printf("\n<---------------------------------------------------------------------------------------->\n");
        sleep(3);

        printf("\n\033[32m%s|memory release\033[0m\n", getDate());
        BuddyAllocator_free(&alloc, p13);

        printf("\n<---------------------------------------------------------------------------------------->\n");


        printf("PROVO AD ALLOCARE 0 BYTES\n");

        sleep(2);

        printf("\n\033[35m%s|memory request\033[0m\n", getDate());
        void *p16 = BuddyAllocator_malloc(&alloc, 0);
        printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p16);

        return 0;
    }

    printf("\n<------------------------------------INIZIO TEST--------------------------------------->\n\n"); 

    sleep(2);
    
    //***



    printf("\n<----------------------------------TEST FULL ALLOC------------------------------------->\n\n");

    sleep(2);

    printf("\n\033[35m%s|memory request\033[0m\n", getDate());
    void *p1 = BuddyAllocator_malloc(&alloc, 125);
    printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p1);

    
        
    printf("\n<---------------------------------------------------------------------------------------->\n");

    sleep(3);

    printf("\n\033[35m%s|memory request\033[0m\n", getDate());
    void *p10 = BuddyAllocator_malloc(&alloc, 252);
    printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p10);

    
        
    printf("\n<---------------------------------------------------------------------------------------->\n");
    sleep(3);

    printf("\n\033[35m%s|memory request\033[0m\n", getDate());
    void *p2 = BuddyAllocator_malloc(&alloc, 508);
    printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p2);


    printf("\n<---------------------------------------------------------------------------------------->\n");
    sleep(3);


    printf("\n\033[32m%s|memory release\033[0m\n", getDate());
    BuddyAllocator_free(&alloc, p1);

    printf("\n<---------------------------------------------------------------------------------------->\n");
    sleep(3);

    printf("\n\033[32m%s|memory release\033[0m\n", getDate());
    BuddyAllocator_free(&alloc, p2);

    printf("\n<---------------------------------------------------------------------------------------->\n");
    sleep(3);

    printf("\n\033[32m%s|memory release\033[0m\n", getDate());
    BuddyAllocator_free(&alloc, p10);

    printf("\n<---------------------------------------------------------------------------------------->\n");
    sleep(3);


    printf("\n\033[35m%s|memory request\033[0m\n", getDate());
    void *p3 = BuddyAllocator_malloc(&alloc, 1020);
    printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p3);

    printf("\n<---------------------------------------------------------------------------------------->\n");
    sleep(3);

    printf("\n\033[32m%s|memory release\033[0m\n", getDate());
    BuddyAllocator_free(&alloc, p3);

    printf("\n<-------------------------------------FINE TEST 1----------------------------------------->\n\n");
    
    sleep(2);
    /***/
    //***

    printf("\n<----------------------------------------TEST 2------------------------------------------->\n\n");

    sleep(2);

    printf("\n\033[35m%s|memory request\033[0m\n", getDate());
    void *p4 = BuddyAllocator_malloc(&alloc, 14);
    printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p4);

    printf("\n<---------------------------------------------------------------------------------------->\n");
    sleep(3);

    printf("\n\033[35m%s|memory request\033[0m\n", getDate());
    void *p5 = BuddyAllocator_malloc(&alloc, 28);
    printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p5);

    printf("\n<---------------------------------------------------------------------------------------->\n");
    sleep(3);

    printf("\n\033[35m%s|memory request\033[0m\n", getDate());
    void *p6 = BuddyAllocator_malloc(&alloc, 4);
    printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p6);

    printf("\n<---------------------------------------------------------------------------------------->\n");
    sleep(3);

    printf("\n\033[35m%s|memory request\033[0m\n", getDate());
    void *p7 = BuddyAllocator_malloc(&alloc, 5);
    printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p7);

    printf("\n<---------------------------------------------------------------------------------------->\n");
    sleep(3);

    printf("\n\033[35m%s|memory request\033[0m\n", getDate());
    void *p8 = BuddyAllocator_malloc(&alloc, 4);
    printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p8);

    printf("\n<---------------------------------------------------------------------------------------->\n");
    sleep(3);

    printf("\n\033[35m%s|memory request\033[0m\n", getDate());
    void *p11 = BuddyAllocator_malloc(&alloc, 18);
    printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p11);

    printf("\n<---------------------------------------------------------------------------------------->\n");
    sleep(3);

    printf("\n\033[32m%s|memory release\033[0m\n", getDate());
    BuddyAllocator_free(&alloc, p5);

    printf("\n<---------------------------------------------------------------------------------------->\n");
    sleep(3);

    printf("\n\033[32m%s|memory release\033[0m\n", getDate());
    BuddyAllocator_free(&alloc, p11);

    printf("\n<---------------------------------------------------------------------------------------->\n");
    sleep(3);

    printf("\n\033[32m%s|memory release\033[0m\n", getDate());
    BuddyAllocator_free(&alloc, p8);

    printf("\n<---------------------------------------------------------------------------------------->\n");
    sleep(3);

    printf("\n\033[32m%s|memory release\033[0m\n", getDate());
    BuddyAllocator_free(&alloc, p7);

    printf("\n<---------------------------------------------------------------------------------------->\n");
    sleep(3);

    printf("\n\033[32m%s|memory release\033[0m\n", getDate());
    BuddyAllocator_free(&alloc, p6);

    printf("\n<---------------------------------------------------------------------------------------->\n");
    sleep(3);

    printf("\n\033[32m%s|memory release\033[0m\n", getDate());
    BuddyAllocator_free(&alloc, p4);

    printf("\n<-------------------------------------FINE TEST 2----------------------------------------->\n\n");

    sleep(2);
    /***/

   //**
    printf("\n<-------------------------------------TEST 3 ERRORI--------------------------------------->\n\n");


    printf("ALLOCO UN BLOCCO MAGGIORE DELLA DIMENSIONE TOTALE\n"); 
    
    sleep(2);

    printf("\n\033[35m%s|memory request\033[0m\n", getDate());
    void *p12 = BuddyAllocator_malloc(&alloc, 10000);
    printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p12);

    printf("\n<---------------------------------------------------------------------------------------->\n");

    

    printf("RIEMPIO LA MEMORIA E PROVO AD ALLOCARE ALTRA MEMORIA\n");
    
    sleep(2);

    printf("\n\033[35m%s|memory request\033[0m\n", getDate());
    void *p13 = BuddyAllocator_malloc(&alloc, 1020);
    printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p13);

    printf("\n<---------------------------------------------------------------------------------------->\n");

    sleep(2);

    printf("\n\033[35m%s|memory request\033[0m\n", getDate());
    void *p14 = BuddyAllocator_malloc(&alloc, 1020);
    printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p14);

    printf("\n<---------------------------------------------------------------------------------------->\n");
    sleep(3);

    printf("\n\033[32m%s|memory release\033[0m\n", getDate());
    BuddyAllocator_free(&alloc, p13);

    printf("\n<---------------------------------------------------------------------------------------->\n");


    printf("PROVO AD ALLOCARE 0 BYTES\n");

    sleep(2);

    printf("\n\033[35m%s|memory request\033[0m\n", getDate());
    void *p16 = BuddyAllocator_malloc(&alloc, 0);
    printf("\n\033[93m%s|memory address [\033[0m\033[34m%p\033[0m\033[93m]\033[0m\n", getDate(), p16);

    return 0;
}
