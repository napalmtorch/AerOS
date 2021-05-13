#include <lib/types.h>
#include <hardware/fat.hpp>
#include <gui/bitmap2.hpp>
#include <hardware/memory.hpp>
extern "C" {
#define size_header()  (uint32_t)default_info.offset_data
#define size_data(width, height) (uint32_t)((height) * ((width)*3 + (width)%4))
#define size_bmp(width, height)  (size_header() + size_data(width, height))

struct BitMapInfo{
    uint16_t magic;
    uint32_t unused;
    uint32_t offset_data;
    uint32_t header_bytes;
    uint16_t color_planes;
    uint16_t color_bpp;
    uint32_t compression;
    int32_t  h_resolution;
    int32_t  v_resolution;
    uint32_t colors_palette;
    uint32_t mean_palette;
};
void *calloc(size_t nobj, size_t size)
{
	size_t real_size = nobj * size;
	void *p = mem_alloc(real_size);
	mem_fill((uint8_t*)p, 0, real_size);

	return p;
}
void free_matrix( void** matrix, uint height ) {

    if ( matrix != NULL ) {
        for( height--; height>0; height-- )
            mem_free( matrix[height] );
        mem_free( matrix[0] );
        mem_free( matrix );
    }
}
void** calloc_matrix( uint width, uint height , uint elem_size ){

    uint i;
    void** matrix = (void**)mem_alloc( sizeof(void*)*height );

    if ( matrix == NULL ) {
        return NULL;
    }

    for( i=0; i<height; i++ ) {
        matrix[i] = calloc( elem_size, width );

        if ( matrix[i] == NULL ) {
            free_matrix( matrix, i );
            return NULL;
        }
    }

    return matrix;
}

static const BitMapInfo default_info = {19778,0,54,40,1,24,0,2835,2835,0,0};


BitMap* new_bmp( uint width, uint height ) {

    BitMap *bmp = (BitMap*)mem_alloc( sizeof(BitMap) );

    if ( bmp == NULL ) {
        return NULL;
    }

    bmp->width = width;
    bmp->height = height;

    bmp->info = (BitMapInfo*)mem_alloc( sizeof(BitMapInfo) );

    if ( bmp->info == NULL ) {
        free_bmp(bmp);
        return NULL;
    }

    *bmp->info = default_info;

    bmp->bitmap = (RGBcolor**)calloc_matrix( width, height, sizeof(RGBcolor) );

    if ( bmp->bitmap == NULL ) {
        free_bmp(bmp);
        return NULL;
    }

    return bmp;
}


void free_bmp( BitMap* bmp ) {

    if ( bmp != NULL ) {
        mem_free( bmp->info );
        free_matrix( (void**)bmp->bitmap, bmp->height );
        mem_free( bmp );
    }
}


#define read( buffer, pos, i, to ) \
            for( to=0, i=sizeof(to)-1; i>=0; i-- ) \
                to |= buffer[pos+i]<<(8*i); \
            pos += sizeof(to)


BitMap* read_bmp( char* file_name ) {

    int pos, i, j;
    uint32_t trash;

    uint8_t* buffer;
    BitMapInfo info;

    uint width, height;

    BitMap* bmp;

    FILE* file = fopen( file_name, "rb" );

    if ( file == NULL ) {
        return NULL;
    }

    buffer = (uint8_t*)mem_alloc( size_header() );

    if ( buffer == NULL ) {
        fclose( file );
        return NULL;
    }

    if ( fread( buffer, sizeof(uint8_t), size_header(), file ) != size_header() ) {

        fclose( file );
        mem_free( buffer );
        return NULL;
    }

    pos = 0;
    read( buffer, pos, i, info.magic );
    read( buffer, pos, i, trash );
    read( buffer, pos, i, info.unused );
    read( buffer, pos, i, info.offset_data );
    read( buffer, pos, i, info.header_bytes );
    read( buffer, pos, i, width );
    read( buffer, pos, i, height );
    read( buffer, pos, i, info.color_planes );
    read( buffer, pos, i, info.color_bpp );
    read( buffer, pos, i, info.compression );
    read( buffer, pos, i, trash );
    read( buffer, pos, i, info.h_resolution );
    read( buffer, pos, i, info.v_resolution );
    read( buffer, pos, i, info.colors_palette );
    read( buffer, pos, i, info.mean_palette );
    mem_free( buffer );

    bmp = new_bmp( width, height );

    if ( bmp == NULL ) {
        fclose( file );
        return NULL;
    }

    *bmp->info = info;

    buffer = (uint8_t*)mem_alloc( size_data(width, height) );

    if ( buffer == NULL ) {
        fclose( file );
        free_bmp( bmp );
        return NULL;
    }

    if ( fread( buffer, sizeof(uint8_t), size_data(width, height), file ) !=
                                                        size_data(width, height) ) {

        fclose( file );
        mem_free( buffer );
        free_bmp( bmp );
        return NULL;
    }

    pos = 0;
    for( i=height-1; i>=0; i-- ) {
        for( j=0; j<(int)width; j++ ) {
            bmp->bitmap[i][j].b = buffer[pos++];
            bmp->bitmap[i][j].g = buffer[pos++];
            bmp->bitmap[i][j].r = buffer[pos++];
        }
        pos += width%4;
    }
    mem_free(buffer);

    fclose( file );

    return bmp;
}


#define write( buffer, pos, i, to ) \
            for( i=sizeof(to)-1; i>=0; i-- ) \
                buffer[pos+i] = (uint8_t)(to>>(8*i)); \
            pos += sizeof(to)

}