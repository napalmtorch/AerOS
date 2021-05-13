/**
 * @file bmp.h
 *
 * @author Dario Sneidermanis
 */

#pragma once

#include <lib/types.h>

extern "C" {
typedef struct {
    uint8_t r,g,b;
} RGBcolor;


typedef struct BitMapInfo BitMapInfo;


typedef struct {
    int width;
    int height;
    RGBcolor** bitmap;
    BitMapInfo*  info;
} BitMap;

BitMap* new_bmp( uint width, uint height );


void free_bmp( BitMap* bmp );


BitMap* read_bmp( char* file_name );

}
