/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1<<12)
#define MAX(x, y) ((x) > (y)? (x) : (y))

#define PACK(size, alloc) ((size) | (alloc))

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define GET(p) (*(unsigned int *)(p)) // get the value from p
#define PUT(p, val) (*(unsigned int *)(p) = (val)) //put the value into p

#define GET_SIZE(p) (GET(p) & ~0x7) // get size of p
#define GET_ALLOC(p) (GET(p) & 0x1) //get alloced? of p

#define HDRP(bp) ((char *)(bp) - WSIZE) //get the head of bp
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) //get the footer of bp

#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) //get the pointer of next block
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE))) //get the pointer of previous block

#define PREV(bp)  (*(char **)(bp)) //get the prev
#define NEXT(bp)  (*(char **)((bp) + WSIZE)) //get the next

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

static char *heap_listp;
static char *root;

static void addBlock(void *bp)
{
	NEXT(bp) = root;	
	//printf("root : %p\n", root);
	//printf("NEXT(bp) : %p\n", NEXT(bp));
	PREV(bp) = NULL;
	//printf("PREV(bp) : %p\n", PREV(bp));
	//printf("NEXT(bp): %p\n", NEXT(bp));
	PREV(NEXT(bp)) = bp;
	//printf("PREV(NEXT(bp)) : %p\n", PREV(NEXT(bp)));
	root = bp;
	//printf("changed root : %p\n", root);
}

static void *coalesce(void *bp)
{
	//printf("coalesce %p\n", bp);

	void *prev, *next;
	//printf("FTRP(PREV_BLKP(bp)) : %p\n", FTRP(PREV_BLKP(bp)));
	//printf("HDRP(NEXT_BLKP(bp)) : %p\n", HDRP(NEXT_BLKP(bp)));
	size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
	size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
	size_t size = GET_SIZE(HDRP(bp));
	//printf("prev_alloc : %d\nnext_alloc : %d\n", prev_alloc, next_alloc);
	//printf("size : %d\n", size);

	if(prev_alloc && next_alloc){ //Case 1
		//printf("case 1 start\n");
		addBlock(bp);
    	//printf("case 1 OK\n");
	}
	else if (prev_alloc && !next_alloc){ //Case 2, add this block to free list and merge with next block
		//printf("case 2 start\n");

		next = NEXT_BLKP(bp);
		//printf("next_BLKP OK\n");
		//printf("PREV(next) : %p\n", PREV(next));
		if(PREV(next))
        	NEXT(PREV(next)) = NEXT(next);
		else
			root = NEXT(next);
		//printf("NEXT(next) : %p\n", NEXT(next));
        PREV(NEXT(next)) = PREV(next);
		//printf("PREV(NEXT(next)) : %p\n", PREV(NEXT(next)));
        addBlock(bp); 
		
		size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
		PUT(HDRP(bp), PACK(size, 0));
		PUT(FTRP(bp), PACK(size, 0));
		//printf("case 2 OK\n");
	}
	else if (!prev_alloc && next_alloc){ //Case 3, merge this block with previous block and add it to free list
		//printf("case 3 start\n");

		prev = PREV_BLKP(bp);
		//printf("PREV(prev) : %p\n", PREV(prev));
        if(PREV(prev))
			NEXT(PREV(prev)) = NEXT(prev);
		else
			root = NEXT(prev);

		//printf("NEXT(prev): %p\n", NEXT(prev));
		//printf("PREV(NEXT(prev)) : %p\n", PREV(NEXT(prev)));
		PREV(NEXT(prev)) = PREV(prev);
		//printf("PREV(NEXT(prev)) : %p\n", PREV(NEXT(prev)));
       	addBlock(prev); 

		size += GET_SIZE(HDRP(PREV_BLKP(bp)));
		PUT(FTRP(bp), PACK(size, 0));
		PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
		bp = prev;
		//printf("case 3 OK\n");
	} else { //Case 4, Merge previous, this, next block together and add it to free list
		//printf("case 4 start\n");

        prev = PREV_BLKP(bp);
        next = NEXT_BLKP(bp);

		if(PREV(prev))
        	NEXT(PREV(prev)) = NEXT(prev);
		else
			root = NEXT(prev);
		PREV(NEXT(prev)) = PREV(prev);

		if(PREV(next))
			NEXT(PREV(next)) = NEXT(next);
		else
			root = NEXT(next);
		PREV(NEXT(next)) = PREV(next);
		NEXT(next) = NULL;
		PREV(next) = NULL;

		addBlock(prev);

		size += GET_SIZE(HDRP(PREV_BLKP(bp)))+GET_SIZE(FTRP(NEXT_BLKP(bp)));
		PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
		PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
		bp = prev;
		//printf("case 4 OK\n");
	}
	return bp;
}


static void *extend_heap(size_t words)
{
	//printf("extend_heap words : %d\n", words);
	char *bp;

	size_t size;

	size = (words%2)?(words+1)*WSIZE:words*WSIZE;
	if ((long)(bp = mem_sbrk(size)) == -1)
		return NULL;
	//printf("extended bp : %p\n", bp);
	PUT(HDRP(bp), PACK(size, 0));
	PUT(FTRP(bp), PACK(size, 0)); //add new free block
	PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); //new epilogue

	//printf("HDRP(bp) : %p\n", HDRP(bp));
	//printf("FTRP(bp) : %p\n", FTRP(bp));
	//printf("root : %p\n", root);
	//printf("prev size : %d\n", GET_SIZE(((char *)(bp) - DSIZE)));
	//printf("PREV_BLKP(bp) : %p\n", PREV_BLKP(bp));

	return coalesce(bp); //coalesce that
}

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
	//printf("mm_init\n");
	if ((heap_listp = mem_sbrk(6*WSIZE)) == (void *)-1)
		return -1;

	PUT(heap_listp, 0);
	PUT(heap_listp + (1*WSIZE), PACK(2*DSIZE, 1));
	PUT(heap_listp + (2*WSIZE), (unsigned int)NULL); //prev pointer
	PUT(heap_listp + (3*WSIZE), (unsigned int)NULL); //next pointer
	PUT(heap_listp + (4*WSIZE), PACK(2*DSIZE, 1));
	PUT(heap_listp + (5*WSIZE), PACK(0, 1));
	heap_listp += (2*WSIZE);

	root = heap_listp;
	if (extend_heap((1<<5)/WSIZE) == NULL)
		return -1;

	return 0;
}

void *find_first(int size)
{
	//printf("start to find_first size : %d\n", size);
    char *ptr = root;
	if(ptr == NULL)
		return NULL;

    while( (NEXT(ptr) != NULL) && (GET_ALLOC(HDRP(ptr)) || (GET_SIZE(HDRP(ptr)) < size))){ //while next pointer is not null and allocated and size is smaller
		ptr = NEXT(ptr);
    }
	if (NEXT(ptr) == NULL && (GET_ALLOC(HDRP(ptr)) || (GET_SIZE(HDRP(ptr)) < size))){
		return NULL;
	}
	return ptr;
}


static void place(void *bp, size_t asize)
{
	//printf("place pointer : %p, asize : %d\n", bp, asize);
	size_t csize = GET_SIZE(HDRP(bp));
	//printf("csize : %d\n", csize);	
	if((csize-asize) >= (2*DSIZE)) {
		PUT(HDRP(bp), PACK(asize, 1));
		PUT(FTRP(bp), PACK(asize, 1));
		if(PREV(bp) != NULL)
			NEXT(PREV(bp)) = NEXT(bp);
		else
			root = NEXT(bp);
		//printf("root : %p\n", root);
		PREV(NEXT(bp)) = PREV(bp);
		//remove this block

		bp = NEXT_BLKP(bp);
		PUT(HDRP(bp), PACK(csize-asize, 0));
		PUT(FTRP(bp), PACK(csize-asize, 0));
		coalesce(bp);
	} else { //no split
		PUT(HDRP(bp), PACK(csize, 1));
		PUT(FTRP(bp), PACK(csize, 1));

		if(PREV(bp) != NULL)
			NEXT(PREV(bp)) = NEXT(bp);
		else
			root = NEXT(bp);
		//printf("root : %p\n", root);
		PREV(NEXT(bp)) = PREV(bp);
	}
	//printf("place end\n\n");
}
/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
	//printf("malloc size : %d\n", size);
    size_t asize;
	size_t extendsize;

	char *bp;
	if (size == 0)
		return NULL;
	if (size <= DSIZE)
		asize = DSIZE*2;
	else
		asize = ALIGN(size) + DSIZE;
		
	bp = find_first(asize);
	if(bp == NULL){
		extendsize = MAX(asize, CHUNKSIZE);
		if((bp = extend_heap(extendsize / WSIZE)) == NULL)
			return NULL;
	}
	place(bp, asize);
	return bp;
}
/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
	//printf("free size : %p\n", ptr);
	size_t size = GET_SIZE(HDRP(ptr));

	PUT(HDRP(ptr), PACK(size, 0));
	PUT(FTRP(ptr), PACK(size, 0));

	coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
	//printf("realloc ptr : %p, size : %d\n", ptr, size);
	size_t asize;
	size_t csize;
	void *next;
	if(ptr == NULL)
		return mm_malloc(size);
	if(size == 0){
		mm_free(ptr);
		return NULL;
	}

	if(size <= DSIZE)
		asize = DSIZE*2;
	else
		asize = ALIGN(size)+DSIZE;

	//printf("asize : %d\n", asize);

	if(asize == GET_SIZE(HDRP(ptr)))
		return ptr;
	else if (asize < GET_SIZE(HDRP(ptr))){
		next = find_first(asize);
		if(next == NULL){ //no fit, just use this block
			csize = GET_SIZE(HDRP(ptr));
			if(csize - asize >= 2*DSIZE){ //split
				PUT(HDRP(ptr), PACK(asize, 1));
				PUT(FTRP(ptr), PACK(asize, 1));

				next = NEXT_BLKP(ptr);
				//printf("NEXT_BLKP : %p\n", next);
				PUT(HDRP(next), PACK(csize-asize, 0));
				PUT(FTRP(next), PACK(csize-asize, 0));
				coalesce(next);
			} else { //no split
				PUT(HDRP(ptr), PACK(csize, 1));
				PUT(FTRP(ptr), PACK(csize, 1));
			}
			return ptr;
		}
		//printf("fit new small block, new malloc\n");
		place(next, asize);
		//printf("now block size : %d\n", GET_SIZE(HDRP(ptr)));
		memcpy(next, ptr, asize-DSIZE);
		mm_free(ptr);
		return next;
	}

	//now asize > GET_SIZE(HDRP(ptr))
	next = NEXT_BLKP(ptr);
	csize = GET_SIZE(HDRP(next)); //csize is next free block size
	//printf("this block size : %d\n", GET_SIZE(HDRP(ptr)));
	//printf("next block ptr : %p\n", next);
	//printf("next block size : %d\n", csize);
	//printf("next block free? : %d\n", !GET_ALLOC(HDRP(next)));
	
	if ( !GET_ALLOC(HDRP(next)) && (csize + GET_SIZE(HDRP(ptr)) >= asize)){ //next block is free and ptr+next block size is enough
		//printf("next block free and size enough\n");
		csize += GET_SIZE(HDRP(ptr)); //now csize is whole available size
		//printf("whole block size : %d\n", csize);
		if(csize-asize >= 2*DSIZE){ //split the remained free block
			//printf("split free block\n");
			if(PREV(next) != NULL)
				NEXT(PREV(next)) = NEXT(next);
			else
				root = NEXT(next);
			//printf("root : %p\n", root);
			PREV(NEXT(next)) = PREV(next);
			//remove next block from free linked list

			PUT(HDRP(ptr), PACK(asize, 1));
			PUT(FTRP(ptr), PACK(asize, 1));
			//printf("new block size : %d\n", GET_SIZE(HDRP(ptr)));

			next = NEXT_BLKP(ptr);	
			//printf("NEXT_BLKP : %p\n", next);
			PUT(HDRP(next), PACK(csize-asize, 0));
			PUT(FTRP(next), PACK(csize-asize, 0));
			//printf("New free block size : %d\n", GET_SIZE(HDRP(next)));
			//printf("coalesce %p\n", next);
			coalesce(next);
		} else { //don't have to split
			//printf("don't split\n");
			if(PREV(next) != NULL)
				NEXT(PREV(next)) = NEXT(next);
			else
				root = NEXT(next);
			//printf("root : %p\n", root);
			PREV(NEXT(next)) = PREV(next); //remove next block from freelist
			
			//rewrite the csize as size
			PUT(HDRP(ptr), PACK(csize, 1));
			PUT(FTRP(ptr), PACK(csize, 1));
		}
		return ptr;
	} else { //next block is allocated or next free block size is not enough
		//printf("not free or not enough size, new malloc\n");
		next = mm_malloc(asize);
		//printf("now block size : %d\n", GET_SIZE(HDRP(ptr)));
		memcpy(next, ptr, GET_SIZE(HDRP(ptr))-DSIZE);
		mm_free(ptr);
		return next;
	}
}
