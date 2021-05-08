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

namespace Graphics
{
    // base class for canvas interface
    class Canvas
    {
        public:
            // clear the screen
            virtual void Clear(Color color) = 0;
            void Clear();

            // draw pixel
            virtual void DrawPixel(uint16_t x, uint16_t y, Color color) = 0;
            void DrawPixel(point_t pos, Color color);

            // draw filled rectangle
            virtual void DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, Color color) = 0;
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
    };
}