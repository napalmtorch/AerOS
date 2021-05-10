#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <hardware/ports.hpp>
#include <graphics/canvas.hpp>
#include <graphics/font.hpp>
#include <gui/widget.hpp>

// border style
typedef enum
{
    BORDER_STYLE_NONE,
    BORDER_STYLE_FIXED,
    BORDER_STYLE_3D,
} BORDER_STYLE;

// dialog result
typedef enum
{
    DIALOG_RESULT_OK,
    DIALOG_RESULT_NO,
    DIALOG_RESULT_YES,
    DIALOG_RESULT_CANCEL,
    DIALOG_RESULT_PENDING,
} DIALOG_RESULT;

namespace System
{
    namespace GUI
    {
        // mouse flags
        typedef struct
        {
            bool Hover, Clicked;
            bool Down, Up;      
        } __attribute__((packed)) MouseEventFlags;

        // visual style structure
        typedef struct
        {
            // name
            char Name[32];
            // colors
            Color Colors[8];
            // border
            BORDER_STYLE BorderStyle;
            uint32_t BorderSize;
            // font
            Graphics::Font* Font;

        } __attribute__((packed)) VisualStyle;

        // taskbar class
        class XServerTaskbar
        {
            public:
                void Update();
                void Draw();
        };

        // xserver base class
        class XServerHost
        {
            public:
                Graphics::VGACanvas Canvas;
                Graphics::Canvas FullCanvas;
                void Initialize();
                void Start(VIDEO_DRIVER driver);
                void Update();
                void Draw();
                bool IsRunning();
            private:
                XServerTaskbar Taskbar;
                bool Running;
                uint32_t FPS, frames, time, last_time;
        };

        // default visual styles
        extern VisualStyle ButtonStyle;
        extern VisualStyle WindowStyle;

        // title bar icons
        extern uint8_t TitleBarIcon_Exit[];
        extern uint8_t TitleBarIcon_Max[];

        // window manager
        namespace WindowManager
        {
            void Initialize();
            void Update();
            void Draw();

            System::List<GUI::Window*> Windows;
        }
    }
}