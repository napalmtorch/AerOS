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
        // title bar exit icon - 9x9
        const uint8_t ICON_TITLEBAR_EXIT_9x9[9 * 9] = 
        {
            1, 1, 0, 0, 0, 0, 0, 1, 1,
            1, 1, 1, 0, 0, 0, 1, 1, 1,
            0, 1, 1, 1, 0, 1, 1, 1, 0,
            0, 0, 1, 1, 1, 1, 1, 0, 0,
            0, 0, 0, 1, 1, 1, 0, 0, 0,
            0, 0, 1, 1, 1, 1, 1, 0, 0,
            0, 1, 1, 1, 0, 1, 1, 1, 0,
            1, 1, 1, 0, 0, 0, 1, 1, 1,
            1, 1, 0, 0, 0, 0, 0, 1, 1,
        };

        // title bar maximize icon - 9x9
        const uint8_t ICON_TITLEBAR_MAX_9x9[9 * 9] = 
        {
            1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1,
        };

        // title bar restore icon - 9x9
        const uint8_t ICON_TITLEBAR_RES_9x9[9 * 9] = 
        {
            0, 0, 1, 1, 1, 1, 1, 1, 1,
            0, 0, 1, 1, 1, 1, 1, 1, 1,
            0, 0, 1, 0, 0, 0, 0, 0, 1,
            1, 1, 1, 1, 1, 1, 1, 0, 1,
            1, 1, 1, 1, 1, 1, 1, 0, 1,
            1, 0, 0, 0, 0, 0, 1, 1, 1,
            1, 0, 0, 0, 0, 0, 1, 0, 0,
            1, 0, 0, 0, 0, 0, 1, 0, 0,
            1, 0, 0, 0, 0, 0, 1, 0, 0,
        };


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
            bool CanMaximize;
            bool CanMinimize;
            bool Moving;
            bool Resizing;
            bool CanDraw;
            bool Active;
            bool ExitRequest;
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
                { 0xFF, 0,    153,  255  },     // title bar accent
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
                virtual void OnLoad();
                virtual void Update();
                virtual void Draw();

            public:
                void SetPosition(int32_t x, int32_t y);
                void SetSize(int32_t w, int32_t h);
                void SetBounds(int32_t x, int32_t y, int32_t w, int32_t h);

            public:
                bounds_t Bounds;
                bounds_t ClientBounds;
                point_t  MaximumSize;
                point_t  MinimumSize;
                WindowFlags Flags;
                VisualStyle Style;
                HAL::MouseFlags MSFlags;
                char* Title;
                char* Name;

            private:
                bounds_t OldBounds;
                bounds_t ResizeBoundsRight;
                bounds_t ResizeBoundsBottom;
                bounds_t CloseBounds, MaxBounds, MinBounds;
                bool CloseHover, CloseDown, CloseClicked;
                bool MaxHover, MaxDown, MaxClicked;
                bool MinHover, MinDown, MinClicked;
                void UpdateClientBounds();

        };
    }
}