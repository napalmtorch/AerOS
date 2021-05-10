#include "gui/xserver.hpp"
#include <core/kernel.hpp>
#include <gui/widget.hpp>
#include <gui/winmgr.hpp>

namespace System
{
    namespace GUI
    {
        // default button style
        VisualStyle ButtonStyle = 
        {
            "Default",
            {
                { 255, 200, 200, 200 },     // background
                { 255, 0,   0,   0   },     // text
                { 255, 255, 255, 255 },     // border top left
                { 255, 154, 151, 147 },     // border inner bottom right
                { 255, 0,   0,   0,  },     // border outer bottom right
                { 255, 0,   0,   0,  },     // unused
                { 255, 0,   0,   0,  },     // unused
                { 255, 0,   0,   0,  },     // unused
            },
            BORDER_STYLE_3D, 1,
            &Graphics::FONT_8x8_SERIF,
        };

        // default window style
        VisualStyle WindowStyle = 
        {
            "Default",
            {
                { 255, 200, 200, 200 },     // background
                { 255, 0,   0,   0   },     // text
                { 255, 255, 255, 255 },     // border top left
                { 255, 154, 151, 147 },     // border inner bottom right
                { 255, 0,   0,   0,  },     // border outer bottom right
                { 255, 0,   0,   255 },     // title bar background
                { 255, 255, 255, 255 },     // title bar text
                { 255, 0,   0,   0,  },     // unused
            },
            BORDER_STYLE_3D, 1,
            &Graphics::FONT_8x8,
        };

        // title bar exit icon
        uint8_t TitleBarIcon_Exit[8 * 7] = 
        {
            1, 1, 0, 0, 0, 0, 1, 1,
            0, 1, 1, 0, 0, 1, 1, 0,
            0, 0, 1, 1, 1, 1, 0, 0,
            0, 0, 0, 1, 1, 0, 0, 0,
            0, 0, 1, 1, 1, 1, 0, 0,
            0, 1, 1, 0, 0, 1, 1, 0,
            1, 1, 0, 0, 0, 0, 1, 1,
        };

        // title bar maximize icon
        uint8_t TitleBarIcon_Max[8 * 7] = 
        {
            1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1,
            1, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 1, 
            1, 0, 0, 0, 0, 0, 0, 1,
            1, 1, 1, 1, 1, 1, 1, 1,
        };

        // performance stats
        char fpsString[16];
        char winCountStr[16];

        // start button
        Button StartBtn;

        // test window
        Window win;

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
            // set driver
            FullCanvas.SetDriver(driver);
            
            // setup mouse
            KernelIO::Mouse.SetBounds(0, 0, KernelIO::VESA.GetWidth(), KernelIO::VESA.GetHeight());
            KernelIO::Mouse.SetPosition(KernelIO::VESA.GetWidth() / 2, KernelIO::VESA.GetHeight() / 2);

            // start button
            StartBtn = Button(3, KernelIO::VESA.GetHeight() - 20, "");
            StartBtn.Base.Bounds.Width = 56;
            StartBtn.Base.Bounds.Height = 17;

            WindowManager::Initialize();

            // test window
            win = Window(128, 128, 320, 240, "Window");
            WindowManager::Start(&win);

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

            StartBtn.Update();

            WindowManager::Update();

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

            // draw window count
            strdec(WindowManager::Windows.Count, winCountStr);
            FullCanvas.DrawString(0, 16, "WINS: ", Graphics::Colors::White, Graphics::FONT_8x16);
            FullCanvas.DrawString(48, 16, winCountStr, Graphics::Colors::White, Graphics::FONT_8x16);

            // draw taskbar
            Taskbar.Draw();

            StartBtn.Draw();
            FullCanvas.DrawString(StartBtn.Base.Bounds.X + 7, StartBtn.Base.Bounds.Y + 5, "Start", ButtonStyle.Colors[1], Graphics::FONT_8x8);

            WindowManager::Draw();

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
            KernelIO::XServer.FullCanvas.DrawFilledRectangle(0, KernelIO::VESA.GetHeight() - 24, KernelIO::VESA.GetWidth(), 24, ButtonStyle.Colors[0]);

            // draw border
            KernelIO::XServer.FullCanvas.DrawFilledRectangle(0, KernelIO::VESA.GetHeight() - 23, KernelIO::VESA.GetWidth(), 1, ButtonStyle.Colors[2]);

            // draw tray
            KernelIO::XServer.FullCanvas.DrawRectangle3D(KernelIO::VESA.GetWidth() - 100, KernelIO::VESA.GetHeight() - 21, 97, 18, ButtonStyle.Colors[3], ButtonStyle.Colors[2], ButtonStyle.Colors[2]);

            // draw time
            char* time = KernelIO::RTC.GetTimeString(false, false);
            KernelIO::XServer.FullCanvas.DrawString(KernelIO::VESA.GetWidth() - (strlen(time) * 8) - 4, KernelIO::VESA.GetHeight() - 16, time, ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF);
        }
    }
}