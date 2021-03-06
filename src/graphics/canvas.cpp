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
            System::KernelIO::VESA.SetPixel(x, y, color);
        }

        // draw pixel
        void DrawPixel(int32_t x, int32_t y, Color color)
        {
            if (x < 0 || y < 0 || x >= System::KernelIO::VESA.GetWidth() || y >= System::KernelIO::VESA.GetHeight()) { return; }
            System::KernelIO::VESA.SetPixel(x, y, Graphics::RGBToPackedValue(color.R, color.G, color.B));
        }

        // draw pixel
        void DrawPixel(point_t pos, Color color) { DrawPixel(pos.X, pos.Y, color); }

        void DrawFilledRectangle(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
        {
            for (int32_t yy = 0; yy < h; yy++)
            {
                for (int32_t xx = 0; xx < w; xx++) { DrawPixel(x + xx, y + yy, color); }
            }
        }

        // draw filled rectangle
        void DrawFilledRectangle(int32_t x, int32_t y, int32_t w, int32_t h, Color color)
        {
            for (int32_t yy = 0; yy < h; yy++)
            {
                for (int32_t xx = 0; xx < w; xx++) { DrawPixel(x + xx, y + yy, color); }
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

        void DrawArray(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t* data)
        {
            for (size_t yy = 0; yy < h; yy++)
            {
                for (size_t xx = 0; xx < w; xx++)
                {
                    DrawPixel(x + xx, y + yy, data[xx + (yy * w)]);
                }
            }
        }

        void DrawArray(int32_t x, int32_t y, int32_t w, int32_t h, Color trans_key, uint32_t* data)
        {
            for (size_t yy = 0; yy < h; yy++)
            {
                for (size_t xx = 0; xx < w; xx++)
                {
                    uint32_t color = data[xx + (yy * w)];
                    if (color != Graphics::RGBToPackedValue(trans_key.R, trans_key.G, trans_key.B)) 
                    { DrawPixel(x + xx, y + yy, color); }
                }
            }
        }

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

        void DrawBitmapFast(int32_t x, int32_t y, Graphics::Bitmap* bitmap)
        {
            if (bitmap == nullptr) { return; }

            int32_t   w = bitmap->Width;
            int32_t   h = bitmap->Height;
            uint32_t* d = (uint32_t*)bitmap->ImageData;
            while (x + w > System::KernelIO::VESA.GetWidth()) { w--; }
            while (y + h > System::KernelIO::VESA.GetHeight()) { h--; }
            
            for (int32_t yy = 0; yy < ((Graphics::Bitmap*)bitmap)->Height; yy++)
            {
                uint8_t* src = (uint8_t*)(d + (yy * bitmap->Width));
                int32_t xx = x;
                if (xx < 0) { xx = 0; }
                while (xx + w >= System::KernelIO::VESA.GetWidth()) { xx--; }
                uint32_t real_offset = (xx + ((y + yy) * System::KernelIO::VESA.GetWidth())) * 4;
                uint8_t* dest = (uint8_t*)(System::KernelIO::VESA.Buffer + real_offset);
                if (y + yy >= System::KernelIO::VESA.GetHeight()) { return; }
                if (y + yy >= 0 && dest >= System::KernelIO::VESA.Buffer && dest < System::KernelIO::VESA.Buffer + (System::KernelIO::VESA.GetWidth() * System::KernelIO::VESA.GetHeight() * 4))
                {
                    if (x >= 0) { mem_copy(src, dest, w * 4); }
                    else { mem_copy(src - (x * 4), dest, (w + x) * 4); }
                }
            }
        }

        // draw bitmap
        void DrawBitmap(int32_t x, int32_t y, Graphics::Bitmap* bitmap)
        {
            if (bitmap == nullptr) { return; }

            int32_t w = bitmap->Width;
            int32_t h = bitmap->Height;

            uint32_t* data = (uint32_t*)bitmap->ImageData;
            for (int32_t yy = 0; yy < h; yy++)
            {
                for (int32_t xx = 0; xx < w; xx++)
                {
                    uint32_t color = data[(xx + (yy * w))];
                    DrawPixel(x + xx, y + yy, color);
                }
            }
        }

        void DrawBitmap(int32_t x, int32_t y, Color trans_key, Graphics::Bitmap* bitmap)
        {
            if (bitmap == nullptr) { return; }

            int32_t w = bitmap->Width;
            int32_t h = bitmap->Height;

            uint32_t* data = (uint32_t*)bitmap->ImageData;
            for (int32_t yy = 0; yy < h; yy++)
            {
                for (int32_t xx = 0; xx < w; xx++)
                {
                    uint32_t color = data[(xx + (yy * w))];
                    if (!IsColorEqual(PackedValueToRGB(color), trans_key)) { DrawPixel(x + xx, y + yy, color); }
                }
            }
        }

        // draw scaled bitmap
        void DrawBitmap(int32_t x, int32_t y, int32_t scale, Graphics::Bitmap* bitmap)
        {
            if (bitmap == nullptr) { return; }
            int32_t xxs = 0, yys = 0;
            for (int32_t yy = bitmap->Height - 1; yy >= 0; yy--)
            {
                for (int32_t xx = bitmap->Width - 1; xx >= 0; xx--)
                {
                    if (bitmap->Depth == COLOR_DEPTH_24)
                    {
                        uint32_t offset = (3 * (xx + (yy * bitmap->Width)));
                        uint32_t color = Graphics::RGBToPackedValue(bitmap->ImageData[offset + 2], bitmap->ImageData[offset + 1], bitmap->ImageData[offset]);
                        DrawFilledRectangle(x + (scale * (bitmap->Width - xx)), y + (scale * (bitmap->Height - yy)), scale, scale, color);
                    }
                    else if (bitmap->Depth == COLOR_DEPTH_32)
                    {
                        uint32_t offset = (4 * (xx + (yy * bitmap->Width)));
                        uint32_t color = Graphics::RGBToPackedValue(bitmap->ImageData[offset + 2], bitmap->ImageData[offset + 1], bitmap->ImageData[offset]);
                        DrawFilledRectangle(x + (scale * (bitmap->Width - xx)), y + (scale * (bitmap->Height - yy)), scale, scale, color);
                    }
                }
            }  
            /*
            int32_t xxs = 0, yys = 0;

            for (int32_t yy = bitmap->Height - 1; yy >= 0; yy--)
            {
                for (int32_t xx = bitmap->Width - 1; xx >= 0; xx--)
                {
                    xxs = (bitmap->Width - xx) * scale;
                    yys = (bitmap->Height - yy) * scale;
                    xxs = (bitmap->Width - xx);
                    yys = (bitmap->Height - yy);

                    uint32_t offset = (4 * (xx + (yy * bitmap->Width)));
                    uint32_t color = Graphics::RGBToPackedValue(bitmap->ImageData[offset + 2], bitmap->ImageData[offset + 1], bitmap->ImageData[offset]);
                    DrawFilledRectangle(x + xxs, y + yys, scale, scale, color);
                }
            } 
            */
        }
    }
}