#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <hardware/ports.hpp>
#include <graphics/canvas.hpp>
#include <graphics/font.hpp>
#include <graphics/colors.hpp>
#include <gui/widget.hpp>
#include <gui/winmgr.hpp>

namespace System
{
    namespace GUI
    {
        struct XServerTaskbarItem
        {
            char Name[24];
            bool Hover, Down;
            Window* Window;
        };

        // taskbar class
        class XServerTaskbar
        {
            public:
                void Update();
                void Draw();
                XServerTaskbarItem* Items[32];
        };

        // xserver base class
        class XServerHost
        {
            public:
                Color BackColor;
                Graphics::VGACanvas Canvas;
                Graphics::Canvas FullCanvas;
                void Initialize();
                void Start(VIDEO_DRIVER driver);
                void Update();
                void Draw();
                bool IsRunning();
                WindowManager WindowMgr;
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
    }
}