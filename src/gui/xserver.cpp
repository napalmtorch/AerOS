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

    void XServerHost::Start()
    {
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
        Canvas.Clear(COL8_DARK_CYAN);

        Canvas.DrawString(0, 0, "FPS: ", COL8_BLACK, Graphics::FONT_3x5);
        Canvas.DrawString(20, 0, fpsString, COL8_BLACK, Graphics::FONT_3x5);
        Canvas.DrawString(0, 194, KernelIO::RTC.GetTimeString(), COL8_BLACK, Graphics::FONT_3x5);
        Canvas.DrawString(px, 90, "PENIS", COL8_DARK_RED, Graphics::FONT_8x16);
        KernelIO::Mouse.Draw();

        px += 0.005;
        if (px >= 320) { px = -40; }

        // swap buffer
        Canvas.Display();
    }

    bool XServerHost::IsRunning() { return Running; }
}