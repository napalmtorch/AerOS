#include <gui/xserver.hpp>
#include <core/kernel.hpp>

namespace System
{
    int32_t time, last_time, fps, frames;
    char fps_string[64];

    void XServerHost::Start()
    {
        Taskbar.Initialize();
        Running = true;
        
        KernelIO::WindowMgr.Initialize();

        GUI::Window* window = new GUI::Window(64, 64, 320, 240, "Testing", "testing");
        KernelIO::WindowMgr.Start(window);

        GUI::Window* window2 = new GUI::Window(256, 190, 320, 240, "ANOTHER TEST", "testing2");
        KernelIO::WindowMgr.Start(window2);
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

        if (KernelIO::Keyboard.IsKeyDown(HAL::Keys::ESCAPE)) { Running = false; Graphics::Canvas::Clear({ 0xFF, 0, 0, 0 }); }   
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
        Style.Colors[0] = { 255, 24, 24, 24 };
    }

    void XTaskbar::Update()
    {
       
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
    }
}