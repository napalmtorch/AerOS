#include "gui/xserver.hpp"
#include <core/kernel.hpp>

namespace System
{
    char fpsString[16];

    void XServerHost::Initialize()
    {
        Running = false;
        FPS = 0;
        time = 1;
    }

    void XServerHost::Start(VIDEO_DRIVER driver)
    {
        FullCanvas.SetDriver(driver);
        Running = true;
    }

    void XServerHost::Update()
    {
        time = KernelIO::RTC.GetSecond();
        frames++;
        if (time != last_time)
        {
            FPS = frames;
            strdec(FPS, fpsString);
            frames = 0;
            last_time = time;
        }
    }

    float px = 0;
    void XServerHost::Draw()
    {
        // clear the screen
        FullCanvas.Clear({ 0xFF, 0x00, 0x7F, 0x7F });

        FullCanvas.DrawString(0, 0, "FPS: ", Graphics::Colors::White, Graphics::FONT_8x16);
        FullCanvas.DrawString(40, 0, fpsString, Graphics::Colors::White, Graphics::FONT_8x16);
        FullCanvas.DrawString(0, 584, KernelIO::RTC.GetTimeString(), Graphics::Colors::White, Graphics::FONT_8x16);
        FullCanvas.DrawString(px, 400, "PENIS", Graphics::Colors::Red, Graphics::FONT_8x16);
        KernelIO::Mouse.Draw();

        px += 0.005;
        if (px >= 800) { px = -40; }

        // swap buffer
        FullCanvas.Display();
    }

    bool XServerHost::IsRunning() { return Running; }
}