#pragma once
#include <hardware/memory.hpp>
extern "C" {
    void memcpy(void*, void*,uint32_t);
    void free(void*);
    int memcmp(const void* a, const void* b, size_t len); 
    void* malloc(size_t);
}