#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <lib/multitasking.h>
#include <hardware/drivers/mouse.hpp>
#include <graphics/canvas.hpp>

namespace System
{
    namespace GUI
    {
        // types of border styles
        typedef enum
        {
            BORDER_STYLE_NONE,
            BORDER_STYLE_ROUNDED,
            BORDER_STYLE_FLAT,
            BORDER_STYLE_3D,
        } BORDER_STYLE;

        typedef struct
        {
            bool Minimized;
            bool Maximized;
            bool Moving;
            bool Resizing;
            bool CanDraw;
        } __attribute__((packed)) WindowFlags;

        typedef struct
        {
            bool Enabled;
            bool Visible;
            bool CanDraw;
        } __attribute__((packed)) WidgetFlags;

        // structure for managing graphical visual style attributes
        struct VisualStyle
        {
            char Name[32];
            Color Colors[8];
            int32_t BorderSize;
            BORDER_STYLE BorderStyle;
            Graphics::Font* Font;
        };

        // pre-defined styles
        static VisualStyle WindowStyle = 
        {
            "Default",
            { 
                { 0xFF, 0x20, 0x20, 0x20 },     // background 
                { 0xFF, 0xFF, 0xFF, 0xFF },     // foreground
                { 0xFF, 0x32, 0x32, 0x32 },     // border
                { 0xFF, 0x10, 0x10, 0x10 },     // title bar background
                { 0xFF, 0xFF, 0xFF, 0xFF },     // title bar foreground
                { 0xFF, 0x00, 0x00, 0x00 },     // unused
                { 0xFF, 0x00, 0x00, 0x00 },     // unused
                { 0xFF, 0x00, 0x00, 0x00 },     // unused
            },
            1, BORDER_STYLE_FLAT,
            &Graphics::FONT_8x8_SERIF,
        };

        static inline void CopyStyle(VisualStyle* src, VisualStyle* dest)
        {
            strcpy(src->Name, dest->Name);
            dest->BorderSize = src->BorderSize;
            dest->BorderStyle = src->BorderStyle;
            dest->Font = src->Font;
            for (size_t i = 0; i < 8; i++) { dest->Colors[i] = src->Colors[i]; }
        }

        class Window
        {
            public:
                Window();
                Window(int32_t x, int32_t y, int32_t w, int32_t h, char* title, char* name);
                virtual ~Window();
                virtual void Update();
                virtual void Draw();

            public:
                void SetPosition(int32_t x, int32_t y);
                void SetSize(int32_t w, int32_t h);
                void SetBounds(int32_t x, int32_t y, int32_t w, int32_t h);

            public:
                bounds_t Bounds;
                bounds_t ClientBounds;
                bounds_t OldBounds;
                WindowFlags Flags;
                VisualStyle Style;
                HAL::MouseFlags MSFlags;
                char* Title;
                char* Name;

        };
    }
}