#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <graphics/canvas.hpp>
#include <gui/widget.hpp>
#include <gui/xserver.hpp>

namespace System
{
    class WindowManager
    {
        // main methods
        public:
            void Initialize();
            void Update();
            void Draw();
            void OnInterrupt();

        // management
        public:
            GUI::Window*  Start(GUI::Window* window);
            bool          Close(GUI::Window* window);
            GUI::Window*  SetActiveWindow(GUI::Window* window);
            GUI::Window*  GetWindow(int32_t index);
            GUI::Window*  GetActiveWindow();
            int32_t       GetWindowIndex(GUI::Window* window);
            GUI::Window*  IsWindowRunning(GUI::Window* window);
            GUI::Window** GetWindowList();
            uint32_t      GetWindowCount();
            bool          IsInitialized();
            bool          IsAnyMaximized();
            void          PrintWindowList();

        // properties
        private:
            GUI::Window*  ActiveWindow;
            GUI::Window** WindowList;
            uint32_t      WindowCount;
            int32_t       WindowIndex;
            uint32_t      MaxWindowCount;
            int32_t       ActiveIndex;
            int32_t       MaximizedIndex;
            bool          Initialized;


    };
}