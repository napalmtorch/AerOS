#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <graphics/canvas.hpp>
#include <gui/widget.hpp>

namespace System
{
    class XTaskbar
    {
        public:
            void Initialize();
            void Update();
            void Draw();

        public:
            bounds_t Bounds;
            GUI::VisualStyle Style;

        private:
            bounds_t BtnBounds;
            bool     BtnHover;
            bool     BtnDown;
            bool     BtnClicked;
    };

    class XServerHost
    {
        public:
            XServerHost();
            void Start();
            void Update();
            void Draw();
            void OnInterrupt();
            bool IsRunning();
            int32_t GetFPS();
            char* GetFPSString();

        private:
            void CalculateFPS();

        private:
            bool Running;
            XTaskbar Taskbar;
    };
}