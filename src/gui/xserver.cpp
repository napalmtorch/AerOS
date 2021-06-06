#include <gui/xserver.hpp>
#include <core/kernel.hpp>
#include <apps/app_term.hpp>

namespace System
{
    int32_t time, last_time, fps, frames;
    char fps_string[64];
    Graphics::Bitmap* ico_computer, *ico_folder, *ico_blank;

    XServerHost::XServerHost()
    {
        Running = false;
    }

    void XServerHost::Start()
    {
        if (!Running)
        {
            Taskbar.Initialize();
            
            KernelIO::WindowMgr.Initialize();

            GUI::Window* window = new GUI::Window(64, 64, 320, 240, "Testing", "testing");
            KernelIO::WindowMgr.Start(window);

            Applications::WinTerminal* term = new Applications::WinTerminal(256, 192);
            KernelIO::WindowMgr.Start(term);

            ico_computer = new Graphics::Bitmap("/sys/resources/term32.bmp");
            ico_folder = new Graphics::Bitmap("/sys/resources/folder32.bmp");
            ico_blank = new Graphics::Bitmap("/sys/resources/blank32.bmp");

            Running = true;
        }
    }

    void XServerHost::Update()
    {
        // get framerate
        CalculateFPS();

        Taskbar.Update();

        KernelIO::WindowMgr.Update();
    }

    void XServerHost::Draw()
    {
        Graphics::Canvas::Clear({ 0xFF, 156, 27, 12 });

        if (ico_computer->Depth == COLOR_DEPTH_32) { Graphics::Canvas::DrawBitmap(16, 36, Graphics::Colors::Magenta, ico_computer); }
        if (ico_folder->Depth == COLOR_DEPTH_32) { Graphics::Canvas::DrawBitmap(16, 76, Graphics::Colors::Magenta, ico_folder); }
        if (ico_blank->Depth == COLOR_DEPTH_32) { Graphics::Canvas::DrawBitmap(16, 116, Graphics::Colors::Magenta, ico_blank); }

        KernelIO::WindowMgr.Draw();

        Taskbar.Draw();

        KernelIO::Mouse.Draw();
    }

    void XServerHost::OnInterrupt()
    {
        KernelIO::WindowMgr.OnInterrupt();
    }

    void XServerHost::CalculateFPS()
    {
        frames++;
        time = KernelIO::RTC.GetSecond();
        if (time != last_time)
        {
            fps = frames;
            frames = 0;
            mem_fill((uint8_t*)fps_string, 0, 64);
            char temp[16];
            strdec(fps, temp);
            strcat(fps_string, temp);
            last_time = time;
        }

        // currently broken
        /*
        if (KernelIO::Keyboard.IsKeyDown(HAL::Keys::ESCAPE)) 
        { 
            Running = false; 
            KernelIO::Keyboard.BufferEnabled = true;
            KernelIO::Keyboard.Event_OnEnterPressed = enter_pressed;
            KernelIO::Keyboard.Buffer = KernelIO::Keyboard.InputBuffer;
            KernelIO::Terminal.Window = nullptr;
            KernelIO::Keyboard.TerminalBuffer = true;
            KernelIO::VESA.Clear(0xFF000000);
            KernelIO::VESA.Render();
        } 
        */  
    }

    bool XServerHost::IsRunning() { return Running; }

    int32_t XServerHost::GetFPS() { return fps; }

    char* XServerHost::GetFPSString() { return fps_string; }

    // taskbar

    void XTaskbar::Initialize()
    {
        // set default bounds
        Bounds.X = 0;
        Bounds.Y = 0;
        Bounds.Width = KernelIO::VESA.GetWidth();
        Bounds.Height = 20;

        // set default style
        Style.Colors[0] = { 255, 24,  24,  24  };
        Style.Colors[1] = { 255, 255, 255, 255 };
        Style.Colors[2] = { 255, 0,   153, 255 };
    }

    void XTaskbar::Update()
    {
        bounds_set(&BtnBounds, Bounds.X, Bounds.Y, 48, Bounds.Height);

        if (bounds_contains(&BtnBounds, KernelIO::Mouse.GetX(), KernelIO::Mouse.GetY()))
        {
            BtnHover = true;
            if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Pressed) { BtnDown = true; }
            else { BtnDown = false; BtnClicked = false; }
        }
        else { BtnHover = false; BtnDown = false; BtnClicked = false; }
    }

    void XTaskbar::Draw()
    {
        // draw background
        Graphics::Canvas::DrawFilledRectangle(Bounds, Style.Colors[0]);
        Graphics::Canvas::DrawFilledRectangle(Bounds.X, Bounds.Y + (Bounds.Height - 1), Bounds.Width, 1, { 255, 48, 48, 48 });

        // draw time
        int32_t time_sw = strlen(KernelIO::RTC.GetTimeString(true, true)) * 8;
        Graphics::Canvas::DrawString((Bounds.Width - time_sw) - 6, (Bounds.Height / 2) - 4, KernelIO::RTC.GetTimeString(true, true), Graphics::Colors::White, Graphics::FONT_8x8_SERIF);

        // draw fps
        int32_t fps_sw = strlen(KernelIO::XServer.GetFPSString()) * 8;
        Graphics::Canvas::DrawString(Bounds.Width - 108,  (Bounds.Height / 2) - 4, fps_string, Graphics::Colors::Green, Graphics::Colors::Black, Graphics::FONT_8x8_SERIF);

        // draw button text
        if (BtnHover) { Graphics::Canvas::DrawString(Bounds.X + 6, Bounds.Y + 6, "AerOS", Style.Colors[2], Graphics::FONT_8x8); }
        else { Graphics::Canvas::DrawString(Bounds.X + 6, Bounds.Y + 6, "AerOS", Style.Colors[1], Graphics::FONT_8x8); }
    }
}