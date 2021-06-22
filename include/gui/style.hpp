#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <hardware/memory.hpp>
#include <graphics/canvas.hpp>

namespace System
{
    namespace GUI
    {
        typedef enum
        {
            BORDER_STYLE_NONE,
            BORDER_STYLE_FIXED,
            BORDER_STYLE_ROUNDED,
            BORDER_STYLE_3D,
        } BORDER_STYLE;

        struct VisualStyle
        {
            char* Name;
            uint32_t BorderSize;
            BORDER_STYLE BorderStyle;
            Color Colors[8];
            Graphics::Font* Font;
        };

        static inline void CopyStyle(VisualStyle* src, VisualStyle* dest)
        {
            dest->Name = src->Name;
            dest->BorderSize = src->BorderSize;
            dest->BorderStyle = src->BorderStyle;
            dest->Font = src->Font;
            for (size_t i = 0; i < 8; i++) { dest->Colors[i] = src->Colors[i]; }

        }

        static VisualStyle WindowStyle = 
        {
            "Default",
            1, BORDER_STYLE_FIXED,
            { 
                { 0xFF, 0xAF, 0xAF, 0xAF },           // background
                { 0xFF, 0x00, 0x00, 0x00 },           // forecolor
                { 0xFF, 0xFF, 0xFF, 0xFF },           // border top left
                { 0xFF, 0x7F, 0x7F, 0x7F },           // border bottom right inner
                { 0xFF, 0x00, 0x00, 0x00 },           // border bottom right outer
                { 0xFF, 0x00, 0x00, 0xFF },           // title bar background
                { 0xFF, 0xFF, 0xFF, 0xFF },           // title bar foreground
                { 0xFF, 0x00, 0x00, 0x00 },
            },
            &Graphics::FONT_8x8_SERIF,
        };
    }
}