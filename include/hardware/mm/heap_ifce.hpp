
#ifndef	HEAP_IFCE_H
#define HEAP_IFCE_H

/* Function type */
typedef void	(*no_mem_handler_t)();
typedef void*	(*get_free_pages_t)(int);

/* Interfaces */
void* heap_malloc(int size);
void heap_free(void* address_to_free);
void heap_init(get_free_pages_t get_free_pages_func,no_mem_handler_t handler);

#endif	/* HEAP_IFCE_H */