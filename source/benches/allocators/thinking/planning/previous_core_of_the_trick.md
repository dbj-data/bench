# The core of the trick

> &copy; 2020 -2021 by dbj@dbj.org

[See it in action here.](https://godbolt.org/z/Px99KneWr)

Is in allocating a "block" as a "char slab" in one `calloc` call. Then "dividing" it into number of chunks to be memory chunks delivered by pool allocator. all of the same size. Something like this.

```c
/// --------------------------------------------------------------
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
/// --------------------------------------------------------------
/* struct chunk { char data[0]{}; }; */
/// 
typedef struct chunk_  chunk ;

struct chunk_ 
{ 
    bool     in_use; 
    chunk *  next; 
    char     data[]; 
} ;
/// --------------------------------------------------------------
int main(int , char**) {

    int chunk_size       = 8 ; // NOTE! 2 is min size for strings! char + '\0'
    int number_of_chunks = 8    ;

    size_t block_chunk_size =  sizeof(chunk) + chunk_size ;

    printf("\nUser requested data size (%d) + sizeof(struct chunk)(%zu) = %zu ", chunk_size , sizeof(chunk), block_chunk_size );
    printf("\nMake sure block chunk size is aligned before use" );
    printf("\nsizeof(intptr_t): %zu, sizes must be aligned to that number", sizeof(intptr_t) );
    printf("\n(block_chunk_size %% 8) == %zu\n", block_chunk_size % 8 );

    char slab[ number_of_chunks * block_chunk_size  ];
    char * const block = (char *)slab; // calloc( number_of_chunks, block_chunk_size  );
    char * block_walker = block ;
    chunk * chunky = 0 ;

    for ( int j = 0; j < number_of_chunks ; ++j)
    {
            chunky = (chunk*)(block_walker) ;
            chunky->in_use = true ;
             chunky->next = (chunk*)( (char*)chunky + (block_chunk_size) );
             char bufy[] = { (char)(65 + j) , '\0' };
             strcpy(chunky->data, bufy ) ;
             // fetch the next 
             block_walker = (char*)( block_walker + block_chunk_size );
    }
     // this is now the last chunk
     chunky->in_use = false ;
     chunky->next = 0 ;
    
    block_walker = block ;
    chunky = 0  ;
    for ( int j = 0; j < number_of_chunks; ++j)
    {
      chunky = (chunk*)(block_walker) ;
       printf("\n[%p]chunky: %s (%s)(%p)", chunky, chunky->data, (chunky->in_use ? "true" : "false"), chunky->next ) ;
      // fetch the next 
      block_walker = (char*)( block_walker + block_chunk_size );
    }


    return 0;
}
```
That will spectaluraly fail at runtime if casting or pointer arithmetics errors are made. Not to mention accidental writing to non allocated memory.

[See it in action here.](https://godbolt.org/z/Px99KneWr)

(c) 2020 - 2021 by dbj@dbj.org CC BY SA 4.0
