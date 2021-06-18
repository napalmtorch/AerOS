#include <hardware/memory.hpp>
extern "C" {
    void memcpy(void*, void*,uint32_t);
    void free(void*);
    void* malloc(size_t);
}