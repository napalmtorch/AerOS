#pragma once
#include <lib/types.h>
extern "C"
{
void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len);
void memset(void *dest, int val, int len);
}