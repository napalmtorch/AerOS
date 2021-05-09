#include "graphics/canvas.hpp"
#include <core/kernel.hpp>

namespace Graphics
{
    // ---------------------------------------- CANVAS BASE ---------------------------------------- // 

    // clear the screen black
    void Canvas::Clear() { Clear(Colors::Black); }

    // draw pixel
    void Canvas::DrawPixel(point_t pos, Color color) { DrawPixel(pos.X, pos.Y, color); }

    // draw filled rectangle
    void Canvas::DrawFilledRectangle(point_t pos, point_t size, Color color) { DrawFilledRectangle(pos.X, pos.Y, size.X, size.Y, color); }

    // draw filled rectangle
    void Canvas::DrawFilledRectangle(bounds_t bounds, Color color) { DrawFilledRectangle(bounds.X, bounds.Y, bounds.Width, bounds.Height, color); }

    // draw rectangle outline
    void Canvas::DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t thickness, Color color)
    {
        DrawFilledRectangle(x, y, w, thickness, color);
        DrawFilledRectangle(x, y + h - thickness, w, thickness, color);
        DrawFilledRectangle(x, y + thickness, thickness, h - (thickness * 2), color);
        DrawFilledRectangle(x + w - thickness, y + thickness, thickness, h - (thickness * 2), color);
    }

    // draw rectangle outline
    void Canvas::DrawRectangle(point_t pos, point_t size, uint16_t thickness, Color color) { DrawRectangle(pos.X, pos.Y, size.X, size.Y, thickness, color); }

    // draw rectangle outline
    void Canvas::DrawRectangle(bounds_t bounds, uint16_t thickness, Color color) { DrawRectangle(bounds.X, bounds.Y, bounds.Width, bounds.Height, thickness, color); }

    // draw character
    void Canvas::DrawChar(uint16_t x, uint16_t y, char c, Color fg, Font font)
    {
        if (c == (char)0x20) { return; }
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
    void Canvas::DrawChar(uint16_t x, uint16_t y, char c, Color fg, Color bg, Font font)
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

    // draw character with background color
    void Canvas::DrawChar(point_t pos, char c, Color fg, Color bg, Font font) { DrawChar(pos.X, pos.Y, c, fg, bg, font); }

    // draw string
    void Canvas::DrawString(uint16_t x, uint16_t y, char* text, Color fg, Font font)
    {
        uint16_t dx = x, dy = y;
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
    void Canvas::DrawString(uint16_t x, uint16_t y, char* text, Color fg, Color bg, Font font)
    {
        uint16_t dx = x, dy = y;
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

    // ---------------------------------------- VGA CANVAS

    // clear
    void VGACanvas::Clear() { System::KernelIO::VGA.Clear(COL8_BLACK); }

    // clear with color
    void VGACanvas::Clear(COL8 color) { System::KernelIO::VGA.Clear(color); }

    // display
    void VGACanvas::Display() { System::KernelIO::VGA.Swap(); }
        
    // draw pixel
    void VGACanvas::DrawPixel(uint16_t x, uint16_t y, COL8 color) { System::KernelIO::VGA.SetPixel(x, y, color); }

    // draw pixel with point
    void VGACanvas::DrawPixel(point_t pos, COL8 color) { System::KernelIO::VGA.SetPixel(pos.X, pos.Y, color);  }

    // draw filled rectangle
    void VGACanvas::DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, COL8 color)
    {
        for (size_t i = 0; i < w * h; i++)
        {
            DrawPixel(x + (i % w), y + (i / w), color);
        }
    }

    void VGACanvas::DrawFilledRectangle(point_t pos, point_t size, COL8 color) { DrawFilledRectangle(pos.X, pos.Y, size.X, size.Y, color); }

    void VGACanvas::DrawFilledRectangle(bounds_t bounds, COL8 color) { DrawFilledRectangle(bounds.X, bounds.Y, bounds.Width, bounds.Height, color); }

    // draw rectangle outline
    void VGACanvas::DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t thickness, COL8 color)
    {
        DrawFilledRectangle(x, y, w, thickness, color);
        DrawFilledRectangle(x, y + h - thickness, w, thickness, color);
        DrawFilledRectangle(x, y + thickness, thickness, h - (thickness * 2), color);
        DrawFilledRectangle(x + w - thickness, y + thickness, thickness, h - (thickness * 2), color);
    }

    void VGACanvas::DrawRectangle(point_t pos, point_t size, uint16_t thickness, COL8 color) { DrawRectangle(pos.X, pos.Y, size.X, size.Y, thickness, color); }

    void VGACanvas::DrawRectangle(bounds_t bounds, uint16_t thickness, COL8 color) { DrawRectangle(bounds.X, bounds.Y, bounds.Width, bounds.Height, thickness, color); }

    // draw character
    void VGACanvas::DrawChar(uint16_t x, uint16_t y, char c, COL8 fg, Font font)
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

    void VGACanvas::DrawChar(uint16_t x, uint16_t y, char c, COL8 fg, COL8 bg, Font font)
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
    void VGACanvas::DrawString(uint16_t x, uint16_t y, char* text, COL8 fg, Font font)
    {
        uint16_t dx = x, dy = y;
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

    void VGACanvas::DrawString(uint16_t x, uint16_t y, char* text, COL8 fg, COL8 bg, Font font)
    {
        uint16_t dx = x, dy = y;
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