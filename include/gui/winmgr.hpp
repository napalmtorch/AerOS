#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <lib/list.hpp>
#include <gui/widget.hpp>

namespace System
{
    namespace GUI
    {
        class WindowManager
        {
            public:
                void Initialize();
                void Update();
                void UpdateCounters();
                void Draw();
                void Open(Window* win);
                void Close(Window* win);
                void PushToTop(Window* win);
            
            public:
                Window* Windows[4096];
                Window* ActiveWindow;
                uint32_t Index;
                uint32_t Count;
                int32_t MovingCount;
                int32_t HoverCount;
                int32_t ResizeCount;
                int32_t FullscreenIndex;
                
        };
    }
}