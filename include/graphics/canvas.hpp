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

            // clear the screen
            void Clear(Color color);
            void Clear(uint32_t color);
            void Clear();

            // display
            void Display();

            // draw pixel
            void DrawPixel(uint16_t x, uint16_t y, Color color);
            void DrawPixel(point_t pos, Color color);

            // draw filled rectangle
            void DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, Color color);
            void DrawFilledRectangle(point_t pos, point_t size, Color color);
            void DrawFilledRectangle(bounds_t bounds, Color color);

            // draw rectangle outline
            void DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t thickness, Color color);
            void DrawRectangle(point_t pos, point_t size, uint16_t thickness, Color color);
            void DrawRectangle(bounds_t bounds, uint16_t thickness, Color color);

            // draw character
            void DrawChar(uint16_t x, uint16_t y, char c, Color fg, Font font);
            void DrawChar(point_t pos, char c, Color fg, Font font);
            void DrawChar(uint16_t x, uint16_t y, char c, Color fg, Color bg, Font font);
            void DrawChar(point_t pos, char c, Color fg, Color bg, Font font);

            // draw string
            void DrawString(uint16_t x, uint16_t y, char* text, Color fg, Font font);
            void DrawString(point_t pos, char* text, Color fg, Font font);
            void DrawString(uint16_t x, uint16_t y, char* text, Color fg, Color bg, Font font);
            void DrawString(point_t pos, char* text, Color fg, Color bg, Font font);
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
            void DrawPixel(uint16_t x, uint16_t y, COL8 color);
            void DrawPixel(point_t pos, COL8 color);

            // draw filled rectangle
            void DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, COL8 color);
            void DrawFilledRectangle(point_t pos, point_t size, COL8 color);
            void DrawFilledRectangle(bounds_t bounds, COL8 color);

            // draw rectangle outline
            void DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t thickness, COL8 color);
            void DrawRectangle(point_t pos, point_t size, uint16_t thickness, COL8 color);
            void DrawRectangle(bounds_t bounds, uint16_t thickness, COL8 color);

            // draw character
            void DrawChar(uint16_t x, uint16_t y, char c, COL8 fg, Font font);
            void DrawChar(point_t pos, char c, COL8 fg, Font font);
            void DrawChar(uint16_t x, uint16_t y, char c, COL8 fg, COL8 bg, Font font);
            void DrawChar(point_t pos, char c, COL8 fg, COL8 bg, Font font);

            // draw string
            void DrawString(uint16_t x, uint16_t y, char* text, COL8 fg, Font font);
            void DrawString(point_t pos, char* text, COL8 fg, Font font);
            void DrawString(uint16_t x, uint16_t y, char* text, COL8 fg, COL8 bg, Font font);
            void DrawString(point_t pos, char* text, COL8 fg, COL8 bg, Font font);
    };
}