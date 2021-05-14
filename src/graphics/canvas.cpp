#include "graphics/canvas.hpp"
#include <core/kernel.hpp>

namespace Graphics
{
    namespace Canvas
    {
        // clear with packed color
        void Clear(uint32_t color)
        {
            System::KernelIO::VESA.Clear(color);
        }

        // clear with color
        void Clear(Color color)
        {          
            System::KernelIO::VESA.Clear(Graphics::RGBToPackedValue(color.R, color.G, color.B));
        }

        // display
        void Display()
        {
            System::KernelIO::VESA.Render();
        }

        void DrawPixel(int32_t x, int32_t y, uint32_t color)
        {
            if (x < 0 || y < 0 || x >= System::KernelIO::VESA.GetWidth() || y >= System::KernelIO::VESA.GetHeight()) { return; }
            if (color == 0) { return; }
            System::KernelIO::VESA.SetPixel(x, y, color);
        }

        // draw pixel
        void DrawPixel(int32_t x, int32_t y, Color color)
        {
            if (color.A == 0) { return; }
            if (x < 0 || y < 0) { return; }
            System::KernelIO::VESA.SetPixel(x, y, Graphics::RGBToPackedValue(color.R, color.G, color.B));
        }

        // draw pixel
        void DrawPixel(point_t pos, Color color) { DrawPixel(pos.X, pos.Y, color); }

        // draw filled rectangle
        void DrawFilledRectangle(int32_t x, int32_t y, int32_t w, int32_t h, Color color)
        {
            if (color.A == 0) { return; }
            for (size_t i = 0; i < w * h; i++)
            {
                DrawPixel(x + (i % w), y + (i / w), color);
            }
        }

        // draw filled rectangle
        void DrawFilledRectangle(point_t pos, point_t size, Color color) { DrawFilledRectangle(pos.X, pos.Y, size.X, size.Y, color); }

        // draw filled rectangle
        void DrawFilledRectangle(bounds_t bounds, Color color) { DrawFilledRectangle(bounds.X, bounds.Y, bounds.Width, bounds.Height, color); }

        // draw rectangle outline
        void DrawRectangle(int32_t x, int32_t y, int32_t w, int32_t h, int32_t thickness, Color color)
        {
            if (color.A == 0) { return; }
            DrawFilledRectangle(x, y, w, thickness, color);
            DrawFilledRectangle(x, y + h - thickness, w, thickness, color);
            DrawFilledRectangle(x, y + thickness, thickness, h - (thickness * 2), color);
            DrawFilledRectangle(x + w - thickness, y + thickness, thickness, h - (thickness * 2), color);
        }

        // draw rectangle outline
        void DrawRectangle(point_t pos, point_t size, int32_t thickness, Color color) { DrawRectangle(pos.X, pos.Y, size.X, size.Y, thickness, color); }

        // draw rectangle outline
        void DrawRectangle(bounds_t bounds, int32_t thickness, Color color) { DrawRectangle(bounds.X, bounds.Y, bounds.Width, bounds.Height, thickness, color); }

        void DrawRectangle3D(int32_t x, int32_t y, int32_t w, int32_t h, Color tl, Color b_inner, Color b_outer)
        {
            DrawFilledRectangle(x, y, w, 1, tl);
            DrawFilledRectangle(x, y, 1, h, tl);
            DrawFilledRectangle(x + 1, y + h - 2, w - 2, 1, b_inner);
            DrawFilledRectangle(x + w - 2, y + 1, 1, h - 2, b_inner);
            DrawFilledRectangle(x, y + h - 1, w, 1, b_outer);
            DrawFilledRectangle(x + w - 1, y, 1, h, b_outer);
        }

        // draw character
        void DrawChar(int32_t x, int32_t y, char c, Color fg, Font font)
        {
            if (fg.A == 0) { return; }
            if (c == 0) { return; }
            if (c == '\0') { return; }
            uint32_t p = font.GetHeight() * c;
            for (size_t cy = 0; cy < font.GetHeight(); cy++)
            {
                for (size_t cx = 0; cx < font.GetWidth(); cx++)
                {
                    if (ConvertByteToBitAddress(font.GetData()[p + cy], cx + 1))
                    { DrawPixel(x + (font.GetWidth() - cx), y + cy, fg); }
                }
            }
        }

        // draw character
        void DrawChar(point_t pos, char c, Color fg, Font font) { DrawChar(pos.X, pos.Y, c, fg, font); }

        // draw character with background color
        void DrawChar(int32_t x, int32_t y, char c, Color fg, Color bg, Font font)
        {
            uint32_t p = font.GetHeight() * c;
            for (size_t cy = 0; cy < font.GetHeight(); cy++)
            {
                for (size_t cx = 0; cx < font.GetWidth(); cx++)
                {
                    if (ConvertByteToBitAddress(font.GetData()[p + cy], cx + 1) == 1)
                    { DrawPixel(x + (font.GetWidth() - cx), y + cy, fg); }
                    else { DrawPixel(x + (font.GetWidth() - cx), y + cy, bg); }
                }
            }
        }

        // draw character with background color
        void DrawChar(point_t pos, char c, Color fg, Color bg, Font font) { DrawChar(pos.X, pos.Y, c, fg, bg, font); }

        // draw string
        void DrawString(int32_t x, int32_t y, char* text, Color fg, Font font)
        {
            if (strlen(text) == 0 || text[0] == '\0') { return; }
            int32_t dx = x, dy = y;
            for (size_t i = 0; i < strlen(text); i++)
            {
                if (text[i] == '\n') { dx = x; dy += font.GetHeight() + font.GetVerticalSpacing(); }
                else if (text[i] >= 32 && text[i] <= 126)
                {
                    DrawChar(dx, dy, text[i], fg, font);
                    dx += font.GetWidth() + font.GetHorizontalSpacing();
                }
            }
        }
        
        // draw string
        void DrawString(point_t pos, char* text, Color fg, Font font) { DrawString(pos.X, pos.Y, text, fg, font); }

        // draw string with background color
        void DrawString(int32_t x, int32_t y, char* text, Color fg, Color bg, Font font)
        {
            int32_t dx = x, dy = y;
            for (size_t i = 0; i < strlen(text); i++)
            {
                if (text[i] == '\n') { dx = x; dy += font.GetHeight() + font.GetVerticalSpacing(); }
                else if (text[i] >= 32 && text[i] <= 126)
                {
                    DrawChar(dx, dy, text[i], fg, bg, font);
                    dx += font.GetWidth() + font.GetHorizontalSpacing();
                }
            }
        }

        // draw string with background color
        void DrawString(point_t pos, char* text, Color fg, Color bg, Font font) { DrawString(pos.X, pos.Y, text, fg, bg, font); }

        // draw array of zeros and ones
        void DrawFlatArray(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t* data, Color color)
        {
            for (size_t yy = 0; yy < h; yy++)
            {
                for (size_t xx = 0; xx < w; xx++)
                {
                    if (data[xx + (yy * w)] > 0) { DrawPixel(x + xx, y + yy, color); }
                }
            }
        }
    }
}