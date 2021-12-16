#include <stdio.h>
#include <assert.h>
#include <math.h> // for floor and log2
#include "buddy_allocator.h"

// these are trivial helpers to support you in case you want
// to do a bitmap implementation
int levelIdx(size_t idx){
  return (int)floor(log2(idx));
};

int buddyIdx(int idx){
  if (idx&0x1){
    return idx-1;
  }
  return idx+1;
}

int parentIdx(int idx){
  return idx/2;
}

int startIdx(int idx){
  return (idx-(1<<levelIdx(idx)));
}

// stampo a schermo l'albero della bitmap
void Bitmap_print(BitMap *bit_map){
    int remain_to_print = 0;
    int lvl = -1; 
    int tot = levelIdx(bit_map->num_bits) - 1;  //numero di livelli totale
    for (int i = 0; i < bit_map->num_bits; i++){  
        if (remain_to_print == 0){ 
            if(lvl==tot) break;
            printf("\nLivello %d \t", ++lvl);     //indice del primo elemento del livello: i
            for (int j = 0; j < (1 << tot) - (1 << lvl); j++) printf(" "); //stampa spazi
            remain_to_print = 1 << lvl;
        }
        printf("%d ", BitMap_bit(bit_map, i));
        remain_to_print--;
    }
    printf("\n");
};

int BuddyAllocator_init(BuddyAllocator* alloc,
                         int num_levels,
                         char* alloc_buf, // buffer per l'allocator
                         int alloc_buf_size,
                         char* bitmap_buf, // buffer per la bitmap
                         int bitmap_buf_size,
                         int min_bucket_size) {
    if (min_bucket_size < 8) {
        printf("Minimum bucket troppo piccolo\n");
        return 0;
    }

    if(num_levels>MAX_LEVELS){
        printf("NUMERO DI LIVELLI MAGGIORE DI 16, INIZIALIZZAZIONE BUDDY ALLOCATOR TERMINATA");
        return 0;
    }
    
    //Generazione del numero di bit necessari per la bit_map
    int num_bits = (1 << (num_levels + 1)) - 1 ; // questo sarà anche il massimo numero di allocazioni possibili
  
    assert ("MEMORIA PER LA BITMAP INSUFFICIENTE!" && bitmap_buf_size>=BitMap_getBytes(num_bits));
    
    // we need room also for level 0
    alloc->num_levels=num_levels;
    alloc->buffer = alloc_buf;
    alloc->buffer_size = alloc_buf_size;
    alloc->min_bucket_size=min_bucket_size;

    printf("\nBUDDY ALLOCATOR INIZIALIZZAZIONE . . .\n\nINFO:\n");
    printf("Memoria gestita: %d bytes\n", alloc_buf_size); // (1<<num_levels)*min_bucket_size  (2^5)*8 = 256 bytes     
    printf("Memoria per la BitMap: %d bytes usati di %d bytes forniti \n", BitMap_getBytes(num_bits), bitmap_buf_size);
    printf("Bit usati per la bitmap: %d\n", num_bits);
    printf("Numero di livelli: %d\n", num_levels);
    printf("Dimensione del minimum bucket: %d\n", min_bucket_size);

    BitMap_init(&alloc->bitmap, num_bits, bitmap_buf);
    printf("\n<-------------------------------Bitmap appena allocata-------------------------------->\n");
    Bitmap_print(&alloc->bitmap);
    printf("\n<------------------------------------------------------------------------------------->\n");
    return 1;

};


//allocates memory
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size) {
    //DA IMPLEMENTARE
    return NULL;
};


//releases allocated memory
void BuddyAllocator_free(BuddyAllocator *alloc, void *mem){
    //DA IMPLEMENTARE
    return NULL;
};