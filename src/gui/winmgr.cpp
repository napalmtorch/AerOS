#include <gui/winmgr.hpp>
#include <core/kernel.hpp>
#include <core/debug.hpp>

namespace System
{
    namespace GUI
    {
        void WindowManager::Initialize()
        {
            Windows = new Window*[4096];
            ActiveWindow = nullptr;
            WindowCount = 0;
            ActiveIndex = -1;
        }

        void WindowManager::Update()
        {
            if (WindowCount == 0) { return; }

            uint32_t hover = 0, moving = 0, resizing = 0;
            int32_t  active, top_hover_index;
            int32_t mx = KernelIO::Mouse.GetX(), my = KernelIO::Mouse.GetY();

            int32_t last_hover = -1;
            for (size_t i = 0; i < WindowCount; i++)
            {
                if (Windows[i] != nullptr)
                {
                    if (Windows[i]->ExitRequest)
                    {
                        Close(Windows[i]);
                        return;
                    }
                    if (WindowCount == 0) { return; }

                    // get window flags
                    if (bounds_contains(&Windows[i]->Bounds, mx, my)) { hover++; }
                    if (Windows[i]->Flags.Moving) { moving++; }
                    if (Windows[i]->Flags.Resizing) { resizing++; }
                    if (Windows[i] == ActiveWindow) { active = i; }
                    Windows[i]->Flags.Active = (Windows[i] == ActiveWindow);
                    if (bounds_contains(&Windows[i]->Bounds, mx, my) && i > top_hover_index) { top_hover_index = i; }

                    if (i > last_hover && i < WindowCount - 1) { last_hover = i; }

                    Windows[i]->Update();
                }
            }

            ActiveIndex = active;

            // if mouse clicked but no window selected then reset active window
            if (hover + moving + resizing == 0 && KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Pressed) { ActiveWindow = nullptr; }

            // set cursor to default if no windows are being hovered
            if (hover == 0 && resizing == 0) { }

            // determine active window
            if (moving == 0 && resizing == 0)
            {
                for (uint32_t i = 0; i < WindowCount; i++)
                {
                    if (Windows[i] != nullptr)
                    {
                        if (ActiveWindow == nullptr)
                        {
                            if (bounds_contains(&Windows[i]->Bounds, mx, my))
                            {
                                if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Pressed)
                                {
                                    SetActiveWindow(GetWindow(i));
                                }
                            }
                        }
                        else
                        {
                            if (bounds_contains(&Windows[i]->Bounds, mx, my))
                            {
                                if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Pressed)
                                {
                                    if (ActiveIndex != i && ActiveWindow != GetWindow(i) && !bounds_contains(&ActiveWindow->Bounds, mx, my)) { SetActiveWindow(GetWindow(i)); }
                                }
                            }
                        }
                    }
                }
            }
        }

        void WindowManager::Draw()
        {
            if (WindowCount == 0) { return; }

            for (size_t i = 0; i < WindowCount; i++)
            {
                if (Windows[i] != nullptr)
                {
                    Windows[i]->Refresh();
                }
            }
        }

        Window* WindowManager::Start(Window* win)
        {
            if (win == nullptr) { KernelIO::ThrowError("Tried to start null window"); return nullptr; }
            if (WindowCount >= 4096) { KernelIO::ThrowError("Maximum amount of running windows reached"); return nullptr; }

            Windows[WindowCount] = win;
            WindowCount++;
            ActiveWindow = win;
            ActiveIndex = GetWindowIndex(win);
            win->Flags.Active = true;
            win->OnLoad();
            return Windows[WindowCount - 1];
        }

        bool WindowManager::Close(Window* win)
        {
            if (win == nullptr) { KernelIO::ThrowError("Tried to close null window"); return false; }
            for (size_t i = 0; i < 4096; i++)
            {
                if (Windows[i] == win)
                {
                    if (ActiveIndex == i || ActiveWindow == win) { ActiveIndex--; ActiveWindow = GetWindow(i); }
                    delete win;
                    Windows[i] = nullptr;
                    return true;
                }
            }

            KernelIO::ThrowError("Cannot close window that is not running");
            return false;
        }

        Window* WindowManager::SetActiveWindow(Window* win)
        {
            if (WindowCount == 0) { return nullptr; }
            // check if window is null
            if (win == nullptr) {  return nullptr; }

            // check if window is running
            if (IsWindowRunning(win) == nullptr) { KernelIO::ThrowError("Tried to close window that is not running"); return nullptr; }

            // get window index
            int32_t index = -1;
            for (size_t i = 0; i < WindowCount; i++) { if (Windows[i] == win) { index = i; break; } }
            if (index < 0 || index >= WindowCount) { KernelIO::ThrowError("Invalid index while setting active window"); return nullptr; }

            // shift windows
            uint8_t* start = (uint8_t*)((uint32_t)Windows + (4 * index));
            uint8_t* end   = (uint8_t*)((uint32_t)Windows + (4 * WindowCount));
            for (size_t i = (uint32_t)start; i < (uint32_t)end; i += 4)
            {
                uint32_t* ptr = (uint32_t*)(i - 1);
                ptr[0] = ptr[1];
            }

            // set active window
            ActiveWindow = win;

            // replace last window
            Windows[WindowCount - 1] = win;
            ActiveIndex = GetWindowIndex(win);

            // return window pointer
            return ActiveWindow;
        }

        Window* WindowManager::GetWindow(int32_t index)
        {
            if (WindowCount == 0) { return nullptr; }
            if (index >= WindowCount) { return nullptr; }
            return Windows[index];
        }

        int32_t WindowManager::GetWindowIndex(GUI::Window* window)
        {
            if (window == nullptr) { return -1; }
            for (size_t i = 0; i < WindowCount; i++)
            {
                if (Windows[i] == window) { return i; }
            }
            return -1;
        }

        // check if specified window is running
        GUI::Window* WindowManager::IsWindowRunning(GUI::Window* window)
        {
            // don't bother if list is empty
            if (WindowCount == 0) { return nullptr; }

            for (size_t i = 0; i < WindowCount; i++)
            {
                if (Windows[i] != nullptr)
                {
                    if (Windows[i] == window) { return Windows[i]; }
                }
            }

            // return nullptr if not running
            return nullptr;
        }
    }
}