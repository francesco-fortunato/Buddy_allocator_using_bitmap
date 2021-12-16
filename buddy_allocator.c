#include <stdio.h>
#include <assert.h>
#include <math.h> // for floor and log2
#include "buddy_allocator.h"

// these are trivial helpers to support you in case you want
// to do a bitmap implementation
int levelIdx(size_t idx){
    return (int)floor(log2(idx+1));
};

int buddyIdx(int idx){ //fratello
  if (idx == 0)
    return 0; //0 non ha fatelli
  else if (idx&0x1)
    return idx + 1; //il fratello di 1 è 2 e viceversa
  return idx - 1;
}

int parentIdx(int idx){
    return (int)(idx-1)/2;
}

//ritorna il primo indice del livello 
int firstIdx(int level){
  return (1 << level)-1; //2^level -1
}

//ritorna lo scarto tra idx e il primo indice del livello
int startIdx(int idx){
    return (idx-(firstIdx(levelIdx(idx))));//idx-firstidx(level(idx))
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
    
    //Nel caso non si usi una potenza di 2, verrà utilizzata meno memoria rispetto a quella disponibile
    if (levelIdx(alloc_buf_size) != log2(alloc_buf_size)){
      printf("LA MEMORIA DEDICATA AL BUFFER, OVVERO %d, NON E' UNA POTENZA DI DUE. \nVERRANNO UTILIZZATI SOLO %d BYTES DEI %d FORNITI.\n", alloc_buf_size, min_bucket_size << num_levels, alloc_buf_size);
      alloc_buf_size = min_bucket_size << num_levels; //la dimensione massima effettiva che può gestire
    }

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
    printf("\nTENTO DI ALLOCARE %d bytes\n", size);
    size += sizeof(int); //sizeof(int) byte usati per l'indice della bitmap
    //controllo sulla dimensione
    if (alloc->buffer_size < size) {
        printf("\nIl blocco da allocare con size %d è più grande di tutta la memoria disponibile!\n", size);
        return NULL;
    }

    //determiniamo il livello della pagina partendo dal livello più basso
    int level_new_block = alloc->num_levels;
    int start_size = alloc->min_bucket_size;
    for (int i = 0; i<level_new_block; i++){
      if (start_size >= size){  //se il la dimensione della memoria richiesta è minore del blocco più piccolo:
        break;
      }
      else{ //altrimenti saliamo di livello
        start_size = start_size*2;  //al livello successivo avremo il doppio del bucket precedente
        level_new_block = level_new_block - 1; //saliamo di livello
      }
    }
    printf("\nLIVELLO BLOCCO SCELTO: %d", level_new_block);

    //troviamo un blocco libero nel livello scelto
    int freeidx=-1;  //free rimarrà -1 se non troviamo nessun blocco libero, altrimenti sarà l'indice del nostro blocco libero
    int j;
    for(j = firstIdx(level_new_block); j < firstIdx(level_new_block+1); j++){
      if (!BitMap_bit(&alloc->bitmap, j)){ //se troviamo un blocco libero
        freeidx=j;
        break;
      }
    }  //se dopo il ciclo for non avremo ottenuto nessun blocco libero ritorneremo NULL

    if(freeidx<0){
      printf("Non ci sono blocchi liberi");
      return NULL;
    }

    //settiamo a 1 il valore del blocco di indice freeidx e i suoi antenati
    Set_status_parents(&alloc->bitmap, freeidx, 1);
    Set_status_children(&alloc->bitmap, freeidx, 1);

    //restituiamo l'indirizzo
    char *indirizzo = alloc->buffer + startIdx(freeidx) * start_size;
    ((int *)indirizzo)[0]=freeidx; //insieme all'indirizzo passiamo l'indice trovato, che ci sarà utile per eseguire la free
    printf("\nAllocato un nuovo blocco di dimensione %d al livello %d utilizzando un blocco di dimensioni %d.\nIndice %d, puntatore %p\n", size, level_new_block, start_size, freeidx, indirizzo+sizeof(int));
    printf("Albero BITMAP dopo l'allocazione:\n");
    Bitmap_print(&alloc->bitmap);
    return (void *)(indirizzo + sizeof(int));
};

//releases allocated memory
void BuddyAllocator_free(BuddyAllocator *alloc, void *mem){
  //DA IMPLEMENTARE
}

void Set_status_parents(BitMap *bit_map, int bit_num, int status){
  BitMap_setBit(bit_map, bit_num, status);
  if (bit_num != 0){
    Set_status_parents(bit_map, parentIdx(bit_num), status);
  }
}

void Set_status_children(BitMap *bit_map, int bit_num, int status){
  if (bit_num < bit_map->num_bits){
    BitMap_setBit(bit_map, bit_num, status);
    Set_status_children(bit_map, bit_num * 2 + 1, status);
    Set_status_children(bit_map, bit_num * 2 + 2, status);
  }
}
