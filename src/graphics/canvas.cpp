#include "graphics/canvas.hpp"
#include <core/kernel.hpp>

namespace Graphics
{
    // ---------------------------------------- CANVAS BASE ---------------------------------------- // 

    void Canvas::SetDriver(VIDEO_DRIVER driver)
    {
        Driver = driver;
    }

    VIDEO_DRIVER Canvas::GetDriver() { return Driver; }

    // clear with color
    void Canvas::Clear(Color color)
    {
        if (Driver == VIDEO_DRIVER_VESA) { System::KernelIO::VESA.Clear(Graphics::RGBToPackedValue(color.R, color.G, color.B)); }
    }

    // clear with packed color
    void Canvas::Clear(uint32_t color)
    {
        if (Driver == VIDEO_DRIVER_VESA) { System::KernelIO::VESA.Clear(color); }
    }

    // clear the screen black
    void Canvas::Clear() { Clear(Colors::Black); }

    // display
    void Canvas::Display()
    {
        if (Driver == VIDEO_DRIVER_VESA) { System::KernelIO::VESA.Render(); }
    }

    void Canvas::DrawPixel(int32_t x, int32_t y, uint32_t color)
    {
        if (x < 0 || y < 0) { return; }
        System::KernelIO::VESA.SetPixel(x, y, color);
    }

    // draw pixel
    void Canvas::DrawPixel(int32_t x, int32_t y, Color color)
    {
        if (color.A == 0) { return; }
        if (x < 0 || y < 0) { return; }
        if (Driver == VIDEO_DRIVER_VESA)
        { System::KernelIO::VESA.SetPixel(x, y, Graphics::RGBToPackedValue(color.R, color.G, color.B)); }
    }

    // draw pixel
    void Canvas::DrawPixel(point_t pos, Color color) { DrawPixel(pos.X, pos.Y, color); }

    // draw filled rectangle
    void Canvas::DrawFilledRectangle(int32_t x, int32_t y, int32_t w, int32_t h, Color color)
    {
        if (color.A == 0) { return; }
        for (size_t i = 0; i < w * h; i++)
        {
            DrawPixel(x + (i % w), y + (i / w), color);
        }
    }

    // draw filled rectangle
    void Canvas::DrawFilledRectangle(point_t pos, point_t size, Color color) { DrawFilledRectangle(pos.X, pos.Y, size.X, size.Y, color); }

    // draw filled rectangle
    void Canvas::DrawFilledRectangle(bounds_t bounds, Color color) { DrawFilledRectangle(bounds.X, bounds.Y, bounds.Width, bounds.Height, color); }

    // draw rectangle outline
    void Canvas::DrawRectangle(int32_t x, int32_t y, int32_t w, int32_t h, int32_t thickness, Color color)
    {
        if (color.A == 0) { return; }
        DrawFilledRectangle(x, y, w, thickness, color);
        DrawFilledRectangle(x, y + h - thickness, w, thickness, color);
        DrawFilledRectangle(x, y + thickness, thickness, h - (thickness * 2), color);
        DrawFilledRectangle(x + w - thickness, y + thickness, thickness, h - (thickness * 2), color);
    }

    // draw rectangle outline
    void Canvas::DrawRectangle(point_t pos, point_t size, int32_t thickness, Color color) { DrawRectangle(pos.X, pos.Y, size.X, size.Y, thickness, color); }

    // draw rectangle outline
    void Canvas::DrawRectangle(bounds_t bounds, int32_t thickness, Color color) { DrawRectangle(bounds.X, bounds.Y, bounds.Width, bounds.Height, thickness, color); }

    void Canvas::DrawRectangle3D(int32_t x, int32_t y, int32_t w, int32_t h, Color tl, Color b_inner, Color b_outer)
    {
        DrawFilledRectangle(x, y, w, 1, tl);
        DrawFilledRectangle(x, y, 1, h, tl);
        DrawFilledRectangle(x + 1, y + h - 2, w - 2, 1, b_inner);
        DrawFilledRectangle(x + w - 2, y + 1, 1, h - 2, b_inner);
        DrawFilledRectangle(x, y + h - 1, w, 1, b_outer);
        DrawFilledRectangle(x + w - 1, y, 1, h, b_outer);
    }

    // draw character
    void Canvas::DrawChar(int32_t x, int32_t y, char c, Color fg, Font font)
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
    void Canvas::DrawChar(point_t pos, char c, Color fg, Font font) { DrawChar(pos.X, pos.Y, c, fg, font); }

    // draw character with background color
    void Canvas::DrawChar(int32_t x, int32_t y, char c, Color fg, Color bg, Font font)
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
    void Canvas::DrawChar(point_t pos, char c, Color fg, Color bg, Font font) { DrawChar(pos.X, pos.Y, c, fg, bg, font); }

    // draw string
    void Canvas::DrawString(int32_t x, int32_t y, char* text, Color fg, Font font)
    {
        if (strlen(text) == 0 || text[0] == '\0') { return; }
        int32_t dx = x, dy = y;
        for (size_t i = 0; i < strlen(text); i++)
        {
            if (text[i] == '\n') { dx = x; dy += font.GetHeight() + font.GetVerticalSpacing(); }
            else
            {
                DrawChar(dx, dy, text[i], fg, font);
                dx += font.GetWidth() + font.GetHorizontalSpacing();
            }
        }
    }
    
    // draw string
    void Canvas::DrawString(point_t pos, char* text, Color fg, Font font) { DrawString(pos.X, pos.Y, text, fg, font); }

    // draw string with background color
    void Canvas::DrawString(int32_t x, int32_t y, char* text, Color fg, Color bg, Font font)
    {
        int32_t dx = x, dy = y;
        for (size_t i = 0; i < strlen(text); i++)
        {
            if (text[i] == '\n') { dx = x; dy += font.GetHeight() + font.GetVerticalSpacing(); }
            else
            {
                DrawChar(dx, dy, text[i], fg, bg, font);
                dx += font.GetWidth() + font.GetHorizontalSpacing();
            }
        }
    }

    // draw string with background color
    void Canvas::DrawString(point_t pos, char* text, Color fg, Color bg, Font font) { DrawString(pos.X, pos.Y, text, fg, bg, font); }

    // draw array of zeros and ones
    void Canvas::DrawFlatArray(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t* data, Color color)
    {
        for (size_t yy = 0; yy < h; yy++)
        {
            for (size_t xx = 0; xx < w; xx++)
            {
                if (data[xx + (yy * w)] > 0) { DrawPixel(x + xx, y + yy, color); }
            }
        }
    }

    // ---------------------------------------- VGA CANVAS

    // clear
    void VGACanvas::Clear() { System::KernelIO::VGA.Clear(COL8_BLACK); }

    // clear with color
    void VGACanvas::Clear(COL8 color) { System::KernelIO::VGA.Clear(color); }

    // display
    void VGACanvas::Display() { System::KernelIO::VGA.Swap(); }
        
    // draw pixel
    void VGACanvas::DrawPixel(int32_t x, int32_t y, COL8 color) { System::KernelIO::VGA.SetPixel(x, y, color); }

    // draw pixel with point
    void VGACanvas::DrawPixel(point_t pos, COL8 color) { System::KernelIO::VGA.SetPixel(pos.X, pos.Y, color);  }

    // draw filled rectangle
    void VGACanvas::DrawFilledRectangle(int32_t x, int32_t y, int32_t w, int32_t h, COL8 color)
    {
        for (size_t i = 0; i < w * h; i++)
        {
            DrawPixel(x + (i % w), y + (i / w), color);
        }
    }

    void VGACanvas::DrawFilledRectangle(point_t pos, point_t size, COL8 color) { DrawFilledRectangle(pos.X, pos.Y, size.X, size.Y, color); }

    void VGACanvas::DrawFilledRectangle(bounds_t bounds, COL8 color) { DrawFilledRectangle(bounds.X, bounds.Y, bounds.Width, bounds.Height, color); }

    // draw rectangle outline
    void VGACanvas::DrawRectangle(int32_t x, int32_t y, int32_t w, int32_t h, int32_t thickness, COL8 color)
    {
        DrawFilledRectangle(x, y, w, thickness, color);
        DrawFilledRectangle(x, y + h - thickness, w, thickness, color);
        DrawFilledRectangle(x, y + thickness, thickness, h - (thickness * 2), color);
        DrawFilledRectangle(x + w - thickness, y + thickness, thickness, h - (thickness * 2), color);
    }

    void VGACanvas::DrawRectangle(point_t pos, point_t size, int32_t thickness, COL8 color) { DrawRectangle(pos.X, pos.Y, size.X, size.Y, thickness, color); }

    void VGACanvas::DrawRectangle(bounds_t bounds, int32_t thickness, COL8 color) { DrawRectangle(bounds.X, bounds.Y, bounds.Width, bounds.Height, thickness, color); }

    // draw character
    void VGACanvas::DrawChar(int32_t x, int32_t y, char c, COL8 fg, Font font)
    {
        if (c == (char)0x20) { return; }
        uint32_t p = font.GetHeight() * c;
        for (size_t cy = 0; cy < font.GetHeight(); cy++)
        {
            for (size_t cx = 0; cx < font.GetWidth(); cx++)
            {
                if (ConvertByteToBitAddress(font.GetData()[p + cy], cx + 1) == 1)
                { DrawPixel(x + (font.GetWidth() - cx), y + cy, fg); }
            }
        }
    }

    void VGACanvas::DrawChar(point_t pos, char c, COL8 fg, Font font) { DrawChar(pos.X, pos.Y, c, fg, font); }

    void VGACanvas::DrawChar(int32_t x, int32_t y, char c, COL8 fg, COL8 bg, Font font)
    {
        if (c == (char)0x20) { return; }
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

    void VGACanvas::DrawChar(point_t pos, char c, COL8 fg, COL8 bg, Font font) { DrawChar(pos.X, pos.Y, c, fg, bg, font); }
    // draw string
    void VGACanvas::DrawString(int32_t x, int32_t y, char* text, COL8 fg, Font font)
    {
        int32_t dx = x, dy = y;
        for (size_t i = 0; i < strlen(text); i++)
        {
            if (text[i] == '\n') { dx = x; dy += font.GetHeight() + font.GetVerticalSpacing(); }
            else
            {
                DrawChar(dx, dy, text[i], fg, font);
                dx += font.GetWidth() + font.GetHorizontalSpacing();
            }
        }
    }

    void VGACanvas::DrawString(point_t pos, char* text, COL8 fg, Font font) { DrawString(pos.X, pos.Y, text, fg, font); }

    void VGACanvas::DrawString(int32_t x, int32_t y, char* text, COL8 fg, COL8 bg, Font font)
    {
        int32_t dx = x, dy = y;
        for (size_t i = 0; i < strlen(text); i++)
        {
            if (text[i] == '\n') { dx = x; dy += font.GetHeight() + font.GetVerticalSpacing(); }
            else
            {
                DrawChar(dx, dy, text[i], fg, bg, font);
                dx += font.GetWidth() + font.GetHorizontalSpacing();
            }
        }
    }

    void VGACanvas::DrawString(point_t pos, char* text, COL8 fg, COL8 bg, Font font) { DrawString(pos.X, pos.Y, text, fg, bg, font); }
}