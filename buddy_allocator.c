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

int parentIdx(int idx){ //padre
    return (int)(idx-1)/2;  
}

//ritorna il primo indice del livello 
int firstIdx(int level){
  return (1 << level)-1; //2^level -1
}

//ritorna l'offset tra il buddy di indice 'idx' e il primo indice del livello
int startIdx(int idx){
    return (idx-(firstIdx(levelIdx(idx))));//idx-firstidx(level(idx))
}

// stampo a schermo l'albero binario della bitmap
void Bitmap_print(BitMap *bit_map){
    int remain_to_print = 0;
    int lvl = -1; 
    int tot = levelIdx(bit_map->num_bits) - 1;  //numero di livelli totale
    for (int i = 0; i < bit_map->num_bits; i++){  
        if (remain_to_print == 0){ //se non rimangono bit da stampare al livello lvl
            if(lvl==tot){ //se siamo arrivati all'ultimo livello stop
              break;
            } 
            printf("\n\033[93mLivello %d: \t\033[0m", ++lvl);     //indice del primo elemento del livello: i
            for (int j = 0; j < (1 << tot) - (1 << lvl); j++){   //stampa degli spazi dopo aver scritto "Livello x:"
              printf(" "); //stampa spazi
            } 
            remain_to_print = 1 << lvl; //al prossimo livello dovremo stampare 2^lvl bit
        }
        if (BitMap_bit(bit_map, i)==0){ //se il blocco è 0 lo stampiamo verde
          printf("\033[32m%d\033[0m ", BitMap_bit(bit_map, i));
        }
        else{   //altrimenti lo stampiamo rosso
          printf("\033[31m%d\033[0m ", BitMap_bit(bit_map, i));
        }
        remain_to_print--;  //1 bit in meno da stampare
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
    int num_bits = (1 << (num_levels + 1)) - 1 ;  //(2^(num_levels+1))-1
  
    assert ("MEMORIA PER LA BITMAP INSUFFICIENTE!" && bitmap_buf_size>=BitMap_getBytes(num_bits));
    
    //Nel caso non si usi una potenza di 2, verrà utilizzata meno memoria rispetto a quella disponibile
    if (levelIdx(alloc_buf_size) != log2(alloc_buf_size)){
      printf("LA MEMORIA DEDICATA AL BUFFER, OVVERO %d, NON E' UNA POTENZA DI DUE. \nVERRANNO UTILIZZATI SOLO %d BYTES DEI %d FORNITI.\n", alloc_buf_size, min_bucket_size << num_levels, alloc_buf_size);
      alloc_buf_size = min_bucket_size << num_levels; //la dimensione massima effettiva che può gestire 
                                                      //alloc_buf_size = min_bucket_size * 2^(num_levels)
    }

    // we need room also for level 0
    alloc->num_levels=num_levels;
    alloc->buffer = alloc_buf;
    alloc->buffer_size = alloc_buf_size;
    alloc->min_bucket_size=min_bucket_size;

    printf("\nBUDDY ALLOCATOR INIZIALIZZAZIONE . . .\n\nINFO:\n");
    printf("Memoria gestita: %d bytes\n", alloc_buf_size); // (1<<num_levels)*min_bucket_size  (2^num_levels)*min_buck_size    
    printf("Memoria per la BitMap: %d bytes usati di %d bytes forniti \n", BitMap_getBytes(num_bits), bitmap_buf_size);
    printf("Bit usati per la bitmap: %d\n", num_bits);
    printf("Numero di livelli: %d\n", num_levels);
    printf("Dimensione del minimum bucket: %d\n", min_bucket_size);

    BitMap_init(&alloc->bitmap, num_bits, bitmap_buf);  //crea bitmap associata al buddy allocator "alloc"
    printf("\n<-------------------------------Bitmap appena allocata-------------------------------->\n");
    Bitmap_print(&alloc->bitmap); 
    printf("\n<------------------------------------------------------------------------------------->\n");
    return 1;
};


//allocates memory
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size) {
    if (size==0){
      printf("NON POSSO ALLOCARE 0 BYTES!");
      return NULL;
    }
    printf("\nTENTO DI ALLOCARE %d BYTES + %ld BYTES DEDICATI ALL'INDICE (TOT. %ld) . . .\n", size, sizeof(int), size+sizeof(int));
    size += sizeof(int); //sizeof(int) byte usati per l'indice della bitmap
    //controllo sulla dimensione
    
    if ((alloc->buffer_size) < size) {
        printf("\nIl blocco da allocare con size %d è più grande di tutta la memoria disponibile! MEMORY FAULT.\n", size);
        return NULL;
    }

    int level_new_block;
    int start_size;

    
    if (size>(alloc->buffer_size)/2){  //se la dimensione richiesta supera la metà della memoria totale, allora sarà il primo livello
      level_new_block=0;
      start_size = alloc->buffer_size; 
    } 
    else if(size<=(alloc->buffer_size)/2 && size>(alloc->buffer_size)/4){ 
      level_new_block=1;
      start_size = (alloc->buffer_size)/2; 
    }
    else{    //determiniamo il livello della pagina partendo dal livello più basso
      level_new_block = alloc->num_levels;
      start_size = alloc->min_bucket_size;
      for (int i = 0; i<=level_new_block; i++){
        if (start_size >= size){  //se la dimensione della memoria richiesta è minore del blocco più piccolo:
          break;
        }
        else{ //altrimenti saliamo di livello
          start_size = start_size*2;  //al livello successivo avremo il doppio del bucket precedente
          level_new_block = level_new_block - 1; //saliamo di livello
        }
      }
    }
    

    
    printf("\nLIVELLO BLOCCO SCELTO: %d", level_new_block);

    //troviamo un blocco libero nel livello scelto
    int freeidx=-1;  //free rimarrà -1 se non troviamo nessun blocco libero, altrimenti sarà l'indice del nostro blocco libero
    int j;
    if ( level_new_block == 0){ //se il livello è il primo
      if (!BitMap_bit(&alloc->bitmap, firstIdx(level_new_block))){ //se il blocco è libero
        freeidx = 0;  //l'indice del blocco da allocare sarà 0
      }
    }
    else{ //altrimenti eseguiamo una ricerca nel livello
      for(j = firstIdx(level_new_block); j < firstIdx(level_new_block+1); j++){
        if (!BitMap_bit(&alloc->bitmap, j)){ //se troviamo un blocco libero
          freeidx=j;
          printf("\t\t\tINDICE DEL BUDDY LIBERO: %d", freeidx);
          break;
        }
      }  //se dopo il ciclo for non avremo ottenuto nessun blocco libero freeidx rimarrà -1 e ritorneremo NULL
    }

    
    if(freeidx==-1){  
      printf("\nNon ci sono blocchi liberi. MEMORY FAULT. \n");
      return NULL;
    }

    //settiamo a 1 il valore del blocco di indice freeidx, dei suoi antenati e dei suoi discendenti
    Set_status_parents(&alloc->bitmap, freeidx, 1);
    Set_status_children(&alloc->bitmap, freeidx, 1);

    //restituiamo l'indirizzo e l'offset
    char *indirizzo = alloc->buffer + startIdx(freeidx) * start_size;
    ((int *)indirizzo)[0]=freeidx; //insieme all'indirizzo passiamo l'indice trovato, che ci sarà utile per eseguire la free
    printf("\nAllocato un nuovo blocco di dimensione %d al livello %d utilizzando un blocco di dimensioni %d.\nIndice %d, puntatore \033[34m%p\033[0m\n", size, level_new_block, start_size, freeidx, indirizzo+sizeof(int));
    printf("Albero BITMAP dopo l'allocazione:\n");
    Bitmap_print(&alloc->bitmap);
    return (void *)(indirizzo + sizeof(int));
};

//releases allocated memory
void BuddyAllocator_free(BuddyAllocator *alloc, void *mem){
  
  if (mem==NULL){
    printf("Non posso liberare un blocco ancora non allocato!");
    return;
  }
  
  printf("\nLibero il blocco puntato da \033[34m%p\033[0m . . .\n", mem);

  // we retrieve the buddy from the system
  int *p = (int *)mem;
  int idx_to_free = p[-1];

  printf("Indice da liberare %d\n", idx_to_free);
  //check deve essere un buddy corretto, calcolo la dim di un buddy a quel livello
  int dim_lvl = alloc->min_bucket_size * (1 << (alloc->num_levels - levelIdx(idx_to_free)));
  char *p_to_free = alloc->buffer + startIdx(idx_to_free) * dim_lvl;
  assert("Puntatore non allineato" && (int *)p_to_free == &p[-1]); //se la dimensione del buddy non è quella che dovrebbe essere, abbiamo un errore
  //bisogna evitare double free
  assert("Double free" && BitMap_bit(&alloc->bitmap, idx_to_free));

  //dopo aver liberato un blocco dobbiamo settare a 0 tutti i suoi discendendi
  Set_status_children(&alloc->bitmap, idx_to_free, 0);
  merge(&alloc->bitmap, idx_to_free); //e se ha buddy libero eseguiamo anche il merge

  printf("Bitmap dopo la free:");
  Bitmap_print(&alloc->bitmap);
}

//setta il bit degli antenati
void Set_status_parents(BitMap *bit_map, int bit_num, int status){
  BitMap_setBit(bit_map, bit_num, status);
  if (bit_num != 0){
    Set_status_parents(bit_map, parentIdx(bit_num), status); //ricorsiva
  }
}

//setta il bit dei discendenti
void Set_status_children(BitMap *bit_map, int bit_num, int status){
  if (bit_num < bit_map->num_bits){ //questo controllo viene fatto per non andare oltre al numero di livelli definito
    BitMap_setBit(bit_map, bit_num, status);
    Set_status_children(bit_map, bit_num * 2 + 1, status);  //figlio sx
    Set_status_children(bit_map, bit_num * 2 + 2, status);  //figlio dx
  }
}

//funzione ricorsiva unisce i buddy fino al livello più alto possibile
void merge(BitMap *bitmap, int idx){
  assert("Non puoi fare il merge su un bit libero" && !BitMap_bit(bitmap, idx)); //deve essere libero
  if (idx == 0) return; //siamo alla root
  int idx_buddy = buddyIdx(idx);
  if (!BitMap_bit(bitmap, idx_buddy)){  //se il buddy è libero
    printf("Il buddy di %d, ovvero %d, è libero: MERGE\n", idx, idx_buddy);
    printf("Eseguo il merge dei buddy %d e %d al livello %d . . .\n", idx, idx_buddy, levelIdx(idx));
    int parent_idx = parentIdx(idx);
    BitMap_setBit(bitmap, parent_idx, 0); //Vera operazione di merge, set del parent a 0
    merge(bitmap, parent_idx); //saliamo di livello
  }
  else{ 
    printf("Il buddy di %d, ovvero %d, NON è libero: NO MERGE\n", idx, idx_buddy);
  }
}
