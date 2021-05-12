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
                Color BackColor;
                Graphics::VGACanvas Canvas;
                Graphics::Canvas FullCanvas;
                void Initialize();
                void Start(VIDEO_DRIVER driver);
                void Update();
                void Draw();
                bool IsRunning();
            private:
                WindowManager WindowMgr;
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