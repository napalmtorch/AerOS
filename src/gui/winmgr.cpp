#include <gui/winmgr.hpp>
#include <core/kernel.hpp>
#include <apps/win_empty.hpp>
#include <apps/win_term.hpp>

namespace System
{
    namespace GUI
    {

        void WindowManager::Initialize()
        {
            Applications::WinTerminal* win = new Applications::WinTerminal(128, 128);
            Applications::WinEmpty* win2 = new Applications::WinEmpty(256, 60);
            Open(win);
            Open(win2);
        }

        bool clicked = false;
        void WindowManager::Update()
        {
            UpdateCounters();

            for (size_t i = 0; i < Index + 1; i++)
            {
                if (Windows[i] != nullptr)
                {
                    if (MovingCount <= 0)
                    {
                        if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Pressed && !clicked && bounds_contains(Windows[i]->Bounds, KernelIO::Mouse.GetX(), KernelIO::Mouse.GetY()) && 
                            !Windows[i]->Flags->Minimized && Windows[i] != ActiveWindow)
                        {
                            if (ActiveWindow != nullptr)
                            {
                                if (HoverCount < 2 && !Windows[i]->Flags->Moving && !Windows[i]->Flags->Resizing)
                                {
                                    ActiveWindow = Windows[i];
                                    clicked = true;
                                }
                            }
                            else if (MovingCount == 0) { ActiveWindow = Windows[i]; clicked = true; }
                        }
                    }


                    // update
                    Windows[i]->Update();
                }
            }

            if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Released) { clicked = false; }
        }

        void WindowManager::UpdateCounters()
        {
            MovingCount = 0;
            HoverCount = 0;
            ResizeCount = 0;
            FullscreenIndex = -1;

            for (size_t i = 0; i < Index + 1; i++)
            {
                if (Windows[i] != nullptr)
                {
                    if (Windows[i]->Flags->Moving) { MovingCount++; }
                    if (Windows[i]->Flags->Resizing) { ResizeCount++; }
                    if (Windows[i]->Flags->Maximized) { FullscreenIndex = i; }
                    if (bounds_contains(Windows[i]->Bounds, KernelIO::Mouse.GetX(), KernelIO::Mouse.GetY())) { HoverCount++; }
                }
            }
        }

        void WindowManager::Draw()
        {
            for (size_t i = 0; i < Index + 1; i++)
            {
                if (Windows[i] != nullptr && Windows[i] != ActiveWindow)
                {
                    Windows[i]->Draw();
                }
            }

            if (ActiveWindow != nullptr) { ActiveWindow->Draw(); }
        }

        void WindowManager::Open(Window* win)
        {
            if (win == nullptr) { KernelIO::ThrowError("Cannot open null window"); return; }
            Windows[Index] = win;
            ActiveWindow = win;
            Index++;
            Count++;
        }

        void WindowManager::Close(Window* win)
        {
            for (size_t i = 0; i < Index + 1; i++)
            {
                if (Windows[i] == win)
                {
                    ActiveWindow = nullptr;
                    delete Windows[i];
                    Windows[i] = nullptr;
                    Count--;
                    return;
                }
            }
        }

        void WindowManager::PushToTop(Window* win)
        {
            uint32_t index = 999999;
            for (size_t i = 0; i < Index + 1; i++)
            {
                if (Windows[i] == win) { index = 0; break; }
            }
        }
    }
}