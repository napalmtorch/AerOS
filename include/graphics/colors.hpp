#include "lib/types.h"

extern "C"
{
    // color depths
    typedef enum
    {
        COLOR_DEPTH_4   = 4,
        COLOR_DEPTH_8   = 8,
        COLOR_DEPTH_16  = 16,
        COLOR_DEPTH_24  = 24,
        COLOR_DEPTH_32  = 32,
    } COLOR_DEPTH;

    // color orders
    typedef enum
    {
        COLOR_ORDER_RGBA,
        COLOR_ORDER_ARGB,
        COLOR_ORDER_BGRA,
        COLOR_ORDER_RGB,
        COLOR_ORDER_BGR,
        COLOR_ORDER_PALETTE_16,
        COLOR_ORDER_PALETTE_256,
    } COLOR_ORDER;

    // 4 bit color enumerator
    typedef enum
    {
        COL4_BLACK,
        COL4_DARK_BLUE,
        COL4_DARK_GREEN,
        COL4_DARK_CYAN,
        COL4_DARK_RED,
        COL4_DARK_MAGENTA,
        COL4_DARK_YELLOW,
        COL4_GRAY,
        COL4_DARK_GRAY,
        COL4_BLUE,
        COL4_GREEN,
        COL4_CYAN,
        COL4_RED,
        COL4_MAGENTA,
        COL4_YELLOW,
        COL4_WHITE,
    } COL4;
}