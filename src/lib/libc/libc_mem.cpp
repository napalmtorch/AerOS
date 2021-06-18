#include <lib/libc/libc_mem.hpp>
extern "C" {
void memcpy(void* dst, void* src,uint32_t len)
{
return mem_copy((uint8_t*)src,(uint8_t*)dst,len);
}
void free(void* data)
{
return mem_free(data);
}

void* malloc(size_t size)
{
return mem_alloc(size);
}
}