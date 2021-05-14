/* DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE */
/* Version 2, December 2004 */
/* Copyright (C) 2012 Chapaev <chapaev@fpscopycatresistance.org> */
/* Everyone is permitted to copy and distribute verbatim or modifiedcopies of this license document, and changing it is allowed as longas the name is changed. */
/* DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION */
/* 0. You just DO WHAT THE F**K YOU WANT TO. */

#include <hardware/mm/heap.hpp>
#include <hardware/mm/heap_ifce.hpp>
#include <hardware/paging.hpp>
#include <core/debug.hpp>

static void*	start;		/* Start of first page */
static void*	end;		/* End of last page */

static no_mem_handler_t		no_mem_handler;
static get_free_pages_t		get_free_pages;

free_list_t	free_list;

/* Get the size of the free block */
static inline int
size_of_free_block(free_list_entry* which_one)
{
	header_t* block = (header_t*)((char*)which_one - sizeof(header_t));
	return block->size;
}

/* Get the header of the free block */
static inline header_t*
header_of_free_block(free_list_entry* which_one)
{
	return which_one==NULL?NULL:(header_t*)(void*)((char*)which_one - sizeof(header_t));
}

/* Move list iterator a step forward */
static inline void
free_list_iterator_forward(free_list_iterator* to_move)
{
	*to_move = (*to_move)->next;
}

/* Insert a block to free list */
static void
free_list_insert(header_t* block_to_insert)
{
	if ( free_list_find(block_to_insert) )  return; /* If block is already in list */
	free_list_position prev = free_list;
	/* Find previous one */
	while ( prev->next != NULL && size_of_free_block(prev->next) < block_to_insert->size )
		free_list_iterator_forward(&prev);
	/* entry to insert */
	free_list_entry* entry_to_insert = (free_list_entry*)block_to_insert + sizeof(header_t);
	/* Insert it */
	entry_to_insert->next = prev->next;
	prev->next = entry_to_insert;
}

/* Delete a entry from free list */
static void
free_list_delete(free_list_position entry_to_delete)
{
	free_list_position prev = free_list;
	/* Find previous one */
	while( prev->next != NULL && prev->next != entry_to_delete )
		free_list_iterator_forward(&prev);
	/* No need to free memory */
	if ( NULL != prev->next )	/* If we found */
		prev->next = prev->next->next;
}

/* Find a block from free list and return the entry */
static free_list_entry*
free_list_find(header_t* to_find)
{
	free_list_iterator to_return = free_list;
	/* Loop until found or arrive end of free list */
	while( to_return != NULL && header_of_free_block(to_return) != to_find )
		free_list_iterator_forward(&to_return);
	/* If not find,to_return will be NULL         
	 * Else it will be the position of the header */
	return to_return;
}

/* Find smallest block can fill the size we need */
static header_t*
free_list_find_can_fill(int size)
{
	free_list_iterator can_fill = free_list;
	while ( can_fill != NULL && header_of_free_block(can_fill)->size < size )
		free_list_iterator_forward(&can_fill);
	return header_of_free_block(can_fill);
}

/* Get pages when out of memory */
/* Call handler until get free pages */
void*
oom_get_free_pages(int how_many)
{
	if ( no_mem_handler != NULL ){
		void* ret = NULL;
		while( NULL == ret ){ /* No free pages yet */
			no_mem_handler();
			ret = GetFreePages(how_many);
			debug_writeln_hex("Free Page: ", (uint32_t)ret);
		} /* WE GOT FREE PAGES!!!! */
		return ret;
	}
	return NULL;		/* oops,No handler,i can do nothing for you */
}

/* Initialise func
 * Reigister functions and initialise a block */
void
heap_init(get_free_pages_t get_free_pages_func,
	  no_mem_handler_t handler)
{

	get_free_pages = get_free_pages_func;
	no_mem_handler = handler;
	/* Get one free page for first block */
	start = get_free_pages(1);
	/* No memory... */
	if ( NULL == start )
		start = oom_get_free_pages(1);
	/* End of current page */
	end = start + PAGE_SIZE;

	/* Protect area in start and end of pages */
	void* prot_area_start	= start + P_SIZE;
	void* prot_area_end	= end	+ P_SIZE;

	*(void**)prot_area_start	= NULL;
	*(void**)prot_area_end		= NULL;
	
	header_t* first_header = (header_t*)start + P_SIZE*2;
	footer_t* first_footer = (footer_t*)end - P_SIZE*2;
	/* Initialise first block */
	*first_footer = first_header;
	first_header->size = PAGE_SIZE - sizeof(header_t) - sizeof(footer_t);
	first_header->in_use = 0; /* Of couse it not in use */
	/* Now we initialise free list */
	free_list = (free_list_t)start;
	free_list->next = (free_list_t)first_header + sizeof(header_t);
	free_list->next->next = NULL;
}

/* Extend heap size
 * Return new block */
header_t*
extend_heap(uint32_t size_to_extend)
{
	/* Round up and change to page numbers */
	if (size_to_extend & 0xFFFFF000)
		size_to_extend = ((size_to_extend & 0xFFFFF000) / PAGE_SIZE) + 1;
	/* Get no pages */
	void* new_pages = get_free_pages(size_to_extend);
	/* oops,No free pages,try to get some */
	if ( NULL == new_pages )
		new_pages = oom_get_free_pages(size_to_extend);
	/* Create new block */
	header_t* new_header = (header_t*)new_pages;
	footer_t* new_footer = (footer_t*)(new_header + PAGE_SIZE - sizeof(footer_t));
	/* A flat memory! */
	if ( end == new_pages ){
		footer_t* orig_footer = (footer_t*)end - P_SIZE*2; /* Jump protect area */
		header_t* orig_header = NULL==orig_footer?NULL:*orig_footer;
		if(orig_header!=NULL&&!(orig_header->in_use))
			merge_block(orig_header,new_header); /* Merge it */
	} else {					     /* We have to constrct new page */
		void* protect_area_start	= new_pages;
		*(void**)protect_area_start	= NULL;
		new_header += P_SIZE;
	}
	void*	protect_area_end	= new_pages + size_to_extend * PAGE_SIZE - P_SIZE; 
	*(void**)protect_area_end	= NULL;
	/* Now we construct new block */
	new_header->size = (header_t*)new_footer - (header_t*)new_header - sizeof(header_t);
	new_header->in_use = 0;
	/* Insert it to free list */
	free_list_insert(new_header);
	/* Update end pointer */
	end = new_pages + size_to_extend * PAGE_SIZE;
	
	return new_header;
}	

/* Spilt one block to two
 * Return first one */
header_t*
spilt_block(header_t* block_to_spilt,int size_of_first_block)
{
	/* Delete the block from free list first */
	free_list_delete(free_list_find(block_to_spilt));
	/* Spilt one to two */
	header_t* first_header = block_to_spilt;
	footer_t* first_footer = (footer_t*)first_header + size_of_first_block + sizeof(header_t);
	header_t* second_header = (header_t*)first_footer + sizeof(footer_t);
	footer_t* second_footer = (footer_t*)first_header + first_header->size + sizeof(header_t);
	/* Construct first block */
	*first_footer = first_header;
	first_header->size = size_of_first_block;
	/* Construct second block */
	*second_footer = second_header;
	second_header->size = (header_t*)second_footer - (header_t*)second_header - sizeof(header_t);
	second_header->in_use = 0;
	/* Insert them to free list */
	free_list_insert(first_header);
	free_list_insert(second_header);
	/* Return first one */
	return first_header;
}

/* Merge two blocks to one
 * Return block merged */
header_t*
merge_block(header_t* first_header,header_t* second_header)
{
	/* Delete both blocks */
	free_list_delete(free_list_find(first_header));
	free_list_delete(free_list_find(second_header));
	footer_t* second_footer = (footer_t*)second_header + sizeof(header_t) + second_header->size;
	*second_footer = first_header;
	first_header->size = (header_t*)second_footer - (header_t*)first_header - sizeof(header_t);
	return first_header;
}	

/* Alloc memory */
void*
alloc_memory(int size)
{
	int new_size = size + sizeof(header_t) + sizeof(footer_t); /* Block size we need */
	header_t* header_to_use = free_list_find_can_fill(new_size); /* Find block */
	if ( NULL == header_to_use )	  /* oops,Out of memory */
		header_to_use = extend_heap(size);
	if ( header_to_use->size > size ) /* We need to spilt block */
		header_to_use = spilt_block(header_to_use,size);
	void* address_to_return = (void*)header_to_use + sizeof(header_t);
	/* The block is not free anymore */
	header_to_use->in_use = 1;
	free_list_delete(free_list_find(header_to_use));
	return address_to_return;
}

void
free_memory(void* address_to_free)
{
	/* Do some merge first */
	header_t* curr_header = (header_t*)address_to_free - sizeof(header_t);
	footer_t* curr_footer = (footer_t*)curr_header + sizeof(header_t) + curr_header->size;
	header_t* next_header = (header_t*)curr_footer + sizeof(footer_t);
	footer_t* prev_footer = (footer_t*)curr_header - sizeof(footer_t);
	header_t* prev_header = *prev_footer;
	/* Previous one isn't in use so we merge it */
	if ( NULL != (void*)prev_header && !(prev_header->in_use) ) 
		curr_header = merge_block(prev_header,curr_header);
	/* Same to next one */
	if ( NULL != *(void**)next_header && !(next_header->in_use) )
		merge_block(curr_header,next_header);
	/* This block isn't in use now */
	curr_header->in_use = 0;
	/* Insert it */
	free_list_insert(curr_header);
}

/* Now we define some interface */
void*
heap_malloc(int size)
{
	return alloc_memory(size);
}

void
heap_free(void* address_to_free)
{
	free_memory(address_to_free);
}
/* That's enough */
