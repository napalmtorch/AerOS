#include "gui/xserver.hpp"
#include <core/kernel.hpp>
#include <gui/widget.hpp>

namespace System
{
    namespace GUI
    {
        // default button style
        VisualStyle ButtonStyle = 
        {
            "Default",
            {
                { 255, 200, 200, 200 },
                { 255, 0,   0,   0   },
                { 255, 255, 255, 255 },
                { 255, 154, 151, 147 },
                { 255, 0,   0,   0,  },
                { 255, 0,   0,   0,  },
                { 255, 0,   0,   0,  },
                { 255, 0,   0,   0,  },
            },
            BORDER_STYLE_3D, 1
        };

        // performance stats
        char fpsString[16];

        // initialize xserver interface
        void XServerHost::Initialize()
        {
            Running = false;
            FPS = 0;
            time = 1;
        }

        // start
        void XServerHost::Start(VIDEO_DRIVER driver)
        {
            FullCanvas.SetDriver(driver);
            KernelIO::Mouse.SetBounds(0, 0, KernelIO::VESA.GetWidth(), KernelIO::VESA.GetHeight());
            KernelIO::Mouse.SetPosition(KernelIO::VESA.GetWidth() / 2, KernelIO::VESA.GetHeight() / 2);

            // running flag
            Running = true;
        }

        // update
        void XServerHost::Update()
        {
            // calculate framerate
            time = KernelIO::RTC.GetSecond();
            frames++;
            if (time != last_time)
            {
                FPS = frames;
                strdec(FPS, fpsString);
                frames = 0;
                last_time = time;
            }

            // update taskbar
            Taskbar.Update();

            // get arrow key mouse movement when enabled
            if (KernelIO::Mouse.GetArrowKeyState()) { KernelIO::Mouse.UpdateArrowKeyMovement(); }
        }

        // draw
        void XServerHost::Draw()
        {
            // clear the screen
            FullCanvas.Clear({ 255, 58, 110, 165 });

            // draw fps
            FullCanvas.DrawString(0, 0, "FPS: ", Graphics::Colors::White, Graphics::FONT_8x16);
            FullCanvas.DrawString(40, 0, fpsString, Graphics::Colors::White, Graphics::FONT_8x16);

            // draw taskbar
            Taskbar.Draw();

            // draw font tests
            //FullCanvas.DrawString(128, 128, "Hello World 1234567890", Graphics::Colors::White, Graphics::FONT_3x5);
            //FullCanvas.DrawString(128, 134, "Hello World 1234567890", Graphics::Colors::White, Graphics::FONT_8x8);
            //FullCanvas.DrawString(128, 142, "Hello World 1234567890", Graphics::Colors::White, Graphics::FONT_8x8_SERIF);
            //FullCanvas.DrawString(128, 150, "Hello World 1234567890", Graphics::Colors::White, Graphics::FONT_8x8_PHEONIX);
            //FullCanvas.DrawString(128, 158, "Hello World 1234567890", Graphics::Colors::White, Graphics::FONT_8x16);
            //FullCanvas.DrawString(128, 174, "Hello World 1234567890", Graphics::Colors::White, Graphics::FONT_8x16_CONSOLAS);

            // draw mouse
            KernelIO::Mouse.Draw();


            // swap buffer
            FullCanvas.Display();
        }

        // check if xserver is running
        bool XServerHost::IsRunning() { return Running; }

        // update taskbar
        void XServerTaskbar::Update()
        {

        }

        // draw taskbar
        void XServerTaskbar::Draw()
        {
            // draw background
            KernelIO::XServer.FullCanvas.DrawFilledRectangle(0, KernelIO::VESA.GetHeight() - 28, KernelIO::VESA.GetWidth(), 28, ButtonStyle.Colors[0]);

            // draw border
            KernelIO::XServer.FullCanvas.DrawFilledRectangle(0, KernelIO::VESA.GetHeight() - 27, KernelIO::VESA.GetWidth(), 1, ButtonStyle.Colors[2]);

            // draw tray
            KernelIO::XServer.FullCanvas.DrawRectangle3D(KernelIO::VESA.GetWidth() - 100, KernelIO::VESA.GetHeight() - 24, 97, 21, ButtonStyle.Colors[3], ButtonStyle.Colors[2], ButtonStyle.Colors[2]);

            // draw time
            KernelIO::XServer.FullCanvas.DrawString(KernelIO::VESA.GetWidth() - 72, KernelIO::VESA.GetHeight() - 18, KernelIO::RTC.GetTimeString(), ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF);
        }
    }
}