#pragma once
#include "lib/types.h"
#include "graphics/colors.hpp"
#include "graphics/font.hpp"
typedef enum
{
    TEXT_ALIGN_LEFT,
    TEXT_ALIGN_CENTER,
    TEXT_ALIGN_RIGHT,
} TEXT_ALIGN;

typedef enum
{
    ALIGN_TOP_LEFT,
    ALIGN_TOP_CENTER,
    ALIGN_TOP_RIGHT,
    ALIGN_MIDDLE_LEFT,
    ALIGN_MIDDLE_CENTER,
    ALIGN_MIDDLE_RIGHT,
    ALIGN_BOTTOM_LEFT,
    ALIGN_BOTTOM_CENTER,
    ALIGN_BOTTOM_RIGHT,
} ALIGN;

typedef enum
{
    VIDEO_DRIVER_VGA,
    VIDEO_DRIVER_VESA,
    VIDEO_DRIVER_VMWARE,
} VIDEO_DRIVER;

namespace Graphics
{
    // base class for canvas interface
    class Canvas
    {
        public:
            void SetDriver(VIDEO_DRIVER driver);
            VIDEO_DRIVER GetDriver();

            // clear the screen
            void Clear(Color color);
            void Clear(uint32_t color);
            void Clear();

            // display
            void Display();

            // draw pixel
            void DrawPixel(int32_t x, int32_t y, uint32_t color);
            void DrawPixel(int32_t x, int32_t y, Color color);
            void DrawPixel(point_t pos, Color color);

            // draw filled rectangle
            void DrawFilledRectangle(int32_t x, int32_t y, int32_t w, int32_t h, Color color);
            void DrawFilledRectangle(point_t pos, point_t size, Color color);
            void DrawFilledRectangle(bounds_t bounds, Color color);

            // draw rectangle outline
            void DrawRectangle(int32_t x, int32_t y, int32_t w, int32_t h, int32_t thickness, Color color);
            void DrawRectangle(point_t pos, point_t size, int32_t thickness, Color color);
            void DrawRectangle(bounds_t bounds, int32_t thickness, Color color);

            void DrawRectangle3D(int32_t x, int32_t y, int32_t w, int32_t h, Color tl, Color b_inner, Color b_outer);

            // draw character
            void DrawChar(int32_t x, int32_t y, char c, Color fg, Font font);
            void DrawChar(point_t pos, char c, Color fg, Font font);
            void DrawChar(int32_t x, int32_t y, char c, Color fg, Color bg, Font font);
            void DrawChar(point_t pos, char c, Color fg, Color bg, Font font);

            // draw string
            void DrawString(int32_t x, int32_t y, char* text, Color fg, Font font);
            void DrawString(point_t pos, char* text, Color fg, Font font);
            void DrawString(int32_t x, int32_t y, char* text, Color fg, Color bg, Font font);
            void DrawString(point_t pos, char* text, Color fg, Color bg, Font font);

            void DrawFlatArray(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t* data, Color color);
        private:
            VIDEO_DRIVER Driver;

    };

    class VGACanvas
    {
        public:
            void Clear();
            void Clear(COL8 color);
            
            // display
            void Display();

            // draw pixel
            void DrawPixel(int32_t x, int32_t y, COL8 color);
            void DrawPixel(point_t pos, COL8 color);

            // draw filled rectangle
            void DrawFilledRectangle(int32_t x, int32_t y, int32_t w, int32_t h, COL8 color);
            void DrawFilledRectangle(point_t pos, point_t size, COL8 color);
            void DrawFilledRectangle(bounds_t bounds, COL8 color);

            // draw rectangle outline
            void DrawRectangle(int32_t x, int32_t y, int32_t w, int32_t h, int32_t thickness, COL8 color);
            void DrawRectangle(point_t pos, point_t size, int32_t thickness, COL8 color);
            void DrawRectangle(bounds_t bounds, int32_t thickness, COL8 color);

            // draw character
            void DrawChar(int32_t x, int32_t y, char c, COL8 fg, Font font);
            void DrawChar(point_t pos, char c, COL8 fg, Font font);
            void DrawChar(int32_t x, int32_t y, char c, COL8 fg, COL8 bg, Font font);
            void DrawChar(point_t pos, char c, COL8 fg, COL8 bg, Font font);

            // draw string
            void DrawString(int32_t x, int32_t y, char* text, COL8 fg, Font font);
            void DrawString(point_t pos, char* text, COL8 fg, Font font);
            void DrawString(int32_t x, int32_t y, char* text, COL8 fg, COL8 bg, Font font);
            void DrawString(point_t pos, char* text, COL8 fg, COL8 bg, Font font);
    };
}