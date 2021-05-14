/* DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE */
/* Version 2, December 2004 */
/* Copyright (C) 2012 Chapaev <chapaev@fpscopycatresistance.org> */
/* Everyone is permitted to copy and distribute verbatim or modifiedcopies of this license document, and changing it is allowed as longas the name is changed. */
/* DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION */
/* 0. You just DO WHAT THE F**K YOU WANT TO. */

#ifndef HEAP_H
#define HEAP_H 1

#ifndef NULL
#define NULL 0
#endif	/* NULL */
#include <lib/types.h>
#define	PAGE_SIZE	0x1000 	/* 4KB */
#define P_SIZE		sizeof(void*)

typedef	unsigned int	uint32_t;	/* ...Or not unsigned int */

/* Free-list entry,also free memory */
typedef struct free_list_entry
{
	struct free_list_entry* next; 	/* Pointer to next entry */
} free_list_entry;

/* Block header */
typedef struct
{
	uint32_t size:31; 		/* Size of block */
	uint32_t in_use:1;		/* In use? */
} header_t;

/* Footer is a pointer to a header */
typedef header_t*	footer_t;

/* Free list type define */
typedef free_list_entry*	free_list_t;
typedef free_list_t		free_list_position;
typedef free_list_position	free_list_iterator;

/* Function declarations */
static void			free_list_insert(header_t* block_to_insert);
static void			free_list_delete(free_list_position entry_to_delete);
static free_list_entry*		free_list_find(header_t* to_find);
static header_t*		free_list_find_can_fill(int size);
header_t*			extend_heap(uint32_t size_to_extend);
header_t*			spilt_block(header_t* block_to_spilt,int size_of_first_block);
header_t*			merge_block(header_t* first_header,header_t* second_header);
void*				oom_get_free_pages(int how_many);
void*				alloc_memory(int size);
void				free_memory(void* address_to_free);

#endif	/* HEAP_H */
