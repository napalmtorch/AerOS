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
            char* Name;
            bounds_t Bounds;
            bool Hover, Down;
            void* Window;
            XServerTaskbarItem();
            XServerTaskbarItem(char* text);
        };

        // taskbar class
        class XServerTaskbar
        {
            public:
                void Initialize();
                void Update();
                void Draw();
                XServerTaskbarItem Items[32];
        };

        // start menu class
        class XServerMenu
        {
            public:
                void Initialize();
                void Update();
                void Draw();
                bool Visible;
                bounds_t Bounds;
                XServerTaskbarItem Items[16];
                uint32_t Count = 0;
                int32_t SelectedIndex = 0;
        };

        // xserver base class
        class XServerHost
        {
            public:
                Color BackColor;
                void Initialize();
                void Start();
                void Update();
                void Draw();
                bool IsRunning();
                WindowManager WindowMgr;
                bool Running;
            private:
                XServerTaskbar Taskbar;
                Button MenuBtn;
                XServerMenu Menu;
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