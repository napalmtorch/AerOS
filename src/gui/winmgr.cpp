#include <gui/winmgr.hpp>
#include <core/kernel.hpp>

namespace System
{
    // initialize window manager interface
    void WindowManager::Initialize()
    {
        // check if already initialized
        if (Initialized) { return; }

        // reset counters
        ActiveWindow = nullptr;
        ActiveIndex = -1;
        WindowCount = 0;
        WindowIndex = 0;
        MaxWindowCount = 4096;

        // initialize window list
        WindowList = new GUI::Window*[MaxWindowCount];

        // set initialization flag
        Initialized = true;
    }

    // update running windows
    void WindowManager::Update()
    {
        if (WindowCount == 0) { return; }

        // disable terminal input if no terminal is open
        if (!streql(ActiveWindow->Name, "terminal"))
        {
            KernelIO::Keyboard.TerminalBuffer = false;
        }

        uint32_t hover = 0;
        uint32_t moving = 0;
        uint32_t resizing = 0;
        int32_t mx = KernelIO::Mouse.GetX(), my = KernelIO::Mouse.GetY();
        int32_t active = ActiveIndex;
        int32_t top_hover_index;

        int32_t last_hover = -1;
        for (size_t i = 0; i < WindowCount; i++)
        {
            if (WindowList[i] != nullptr)
            {
                // get window flags
                if (bounds_contains(&WindowList[i]->Bounds, mx, my)) { hover++; }
                if (WindowList[i]->Flags.Moving) { moving++; }
                if (WindowList[i]->Flags.Resizing) { resizing++; }
                if (WindowList[i] == ActiveWindow) { active = i; }
                WindowList[i]->Flags.Active = (WindowList[i] == ActiveWindow);
                if (bounds_contains(&WindowList[i]->Bounds, mx, my) && i > top_hover_index) { top_hover_index = i; }

                if (i > last_hover && i < WindowCount - 1) { last_hover = i; }

                // update window
                WindowList[i]->Update();

                if (WindowList[i]->Flags.ExitRequest)
                {
                    Close(WindowList[i]);
                    return;
                }
            }
        }

        ActiveIndex = active;

        // if mouse clicked but no window selected then reset active window
        if (hover + moving == 0 && KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Pressed) { ActiveWindow = nullptr; }

        // determine active window
        if (moving == 0)
        {
            for (uint32_t i = 0; i < WindowCount; i++)
            {
                if (WindowList[i] != nullptr)
                {
                    if (ActiveWindow == nullptr)
                    {
                        if (bounds_contains(&WindowList[i]->Bounds, mx, my))
                        {
                            if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Pressed)
                            {
                                SetActiveWindow(GetWindow(i));
                            }
                        }
                    }
                    else
                    {
                        if (bounds_contains(&WindowList[i]->Bounds, mx, my))
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

    // draw running windows
    void WindowManager::Draw()
    {
        if (WindowCount == 0) { return; }

        // loop through windows
        for (size_t i = 0; i < WindowCount; i++)
        {
            if (WindowList[i] != nullptr)
            {
                if (!WindowList[i]->Flags.Minimized) { WindowList[i]->Draw(); }
            }
        }

        // print varius information
        char header[64];
        char temp[32];
        header[0] = '\0'; temp[0] = '\0';
        strcat(header, "ACTIVE: ");
        strdec(ActiveIndex, temp);
        strcat(header, temp);
        Graphics::Canvas::DrawString(8, 32, header, Graphics::Colors::White, Graphics::FONT_8x8_SERIF);
    }

    // executed on pit interrupt
    void WindowManager::OnInterrupt()
    {
        
    }

    // start new window 
    GUI::Window* WindowManager::Start(GUI::Window* window)
    {
        // check if window is null
        if (window == nullptr) { KernelIO::ThrowError("Tried to start null window"); return nullptr; }

        // check if window is already running
        if (IsWindowRunning(window)) { KernelIO::ThrowError("Tried to start window that is already opened"); return nullptr; }

        // check if position in list is null
        if (WindowList[WindowIndex] != nullptr) { KernelIO::ThrowError("Tried to start window at existing window pointer"); return nullptr; }

        // add window to list
        WindowList[WindowIndex] = window;
        window->OnLoad();
        window->Flags.Active = true;
        window->Update();
        window->Flags.Active = false;

        // message
        KernelIO::Write("Opened window: ", COL4_GREEN);
        KernelIO::WriteLine(window->Name);

        // increment and return window
        WindowIndex++;
        WindowCount++;
        ActiveWindow = window;
        SetActiveWindow(WindowList[WindowIndex - 1]);
        return window;
    }

    // close running window
    bool WindowManager::Close(GUI::Window* window)
    {
        // check if window is null
        if (window == nullptr) { KernelIO::ThrowError("Tried to close null window"); return false; }

        // check if window is running
        if (!IsWindowRunning(window)) { KernelIO::ThrowError("Tried to close window that is not running"); return false; }

        // get window index
        uint32_t index = GetWindowIndex(window);

        // message
        KernelIO::Write("Closed window: ", COL4_RED);
        KernelIO::WriteLine(window->Name);

        // clear item
        window->~Window();
        WindowList[index] = nullptr;

        // shift windows
        uint8_t* start = (uint8_t*)((uint32_t)WindowList + (4 * index));
        uint8_t* end   = (uint8_t*)((uint32_t)WindowList + (4 * WindowCount));
        for (size_t i = (uint32_t)start; i < (uint32_t)end; i += 4)
        {
            uint32_t* ptr = (uint32_t*)(i + 1);
            ptr[0] = ptr[1];
        }

        // decrement values
        WindowIndex--;
        WindowCount--;
        if (ActiveWindow == window && WindowIndex < MaxWindowCount) { SetActiveWindow(WindowList[WindowCount - 1]); }
        else { ActiveWindow = nullptr; }
        KernelIO::Write("REAL CLOSE");
        return true;
    }

    // set active window
    GUI::Window* WindowManager::SetActiveWindow(GUI::Window* window)
    {
        // check if window is null
        if (window == nullptr) { KernelIO::ThrowError("Tried to close null window"); return nullptr; }

        // check if window is running
        if (!IsWindowRunning(window)) { KernelIO::ThrowError("Tried to close window that is not running"); return nullptr; }

        // get window index
        int32_t index = -1;
        for (size_t i = 0; i < WindowCount; i++) { if (WindowList[i] == window) { index = i; break; } }
        if (index < 0 || index >= WindowCount) { KernelIO::ThrowError("Invalid index while setting active window"); return nullptr; }

        // shift windows
        uint8_t* start = (uint8_t*)((uint32_t)WindowList + (4 * index));
        uint8_t* end   = (uint8_t*)((uint32_t)WindowList + (4 * WindowCount));
        for (size_t i = (uint32_t)start; i < (uint32_t)end; i += 4)
        {
            uint32_t* ptr = (uint32_t*)(i - 1);
            ptr[0] = ptr[1];
        }

        // set active window
        ActiveWindow = window;

        // replace last window
        WindowList[WindowCount - 1] = window;
        ActiveIndex = GetWindowIndex(window);

        // return window pointer
        return ActiveWindow;

    }

    // get running window at index in list
    GUI::Window* WindowManager::GetWindow(int32_t index)
    {
        // don't bother if list is empty
        if (WindowCount == 0) { return nullptr; }

        // check if index is valid
        if (index >= WindowCount) { KernelIO::ThrowError("Window manager index out of bounds while fetching window"); return nullptr; }

        // return window
        return WindowList[index];
    }

    GUI::Window* WindowManager::GetActiveWindow() { return ActiveWindow; }

    int32_t WindowManager::GetWindowIndex(GUI::Window* window)
    {
        if (window == nullptr) { return -1; }
        for (size_t i = 0; i < WindowCount; i++)
        {
            if (WindowList[i] == window) { return i; }
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
            if (WindowList[i] == window) { return WindowList[i]; }
        }

        // return nullptr if not running
        return nullptr;
    }

    // return list of windows
    GUI::Window** WindowManager::GetWindowList() { return WindowList; }

    // get amount of windows in list
    uint32_t WindowManager::GetWindowCount() { return WindowCount; }

    // check if window manager is initialized
    bool WindowManager::IsInitialized() { return Initialized; }

    void WindowManager::PrintWindowList()
    {
        char temp[32];
        for (size_t i = 0; i < WindowCount; i++)
        {
            if (WindowList[i] != nullptr)
            {
                KernelIO::Terminal.Write("PID: ", Graphics::Colors::Yellow);
                strdec(i, temp);
                KernelIO::Terminal.Write(temp);
                KernelIO::Terminal.Write("  NAME: ", Graphics::Colors::Cyan);
                KernelIO::Terminal.WriteLine(WindowList[i]->Name);
            }
        }
    }
}