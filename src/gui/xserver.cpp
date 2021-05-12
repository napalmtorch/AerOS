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

        // initialize xserver interface
        void XServerHost::Initialize()
        {
            Running = false;
            FPS = 0;
            time = 1;
            BackColor = Graphics::Colors::DarkCyan;
            Taskbar.Initialize();
        }

        // start
        void XServerHost::Start(VIDEO_DRIVER driver)
        {
            // set driver
            FullCanvas.SetDriver(driver);
            
            // setup mouse
            KernelIO::Mouse.SetBounds(0, 0, KernelIO::VESA.GetWidth(), KernelIO::VESA.GetHeight());
            KernelIO::Mouse.SetPosition(KernelIO::VESA.GetWidth() / 2, KernelIO::VESA.GetHeight() / 2);

            // test window
            //Applications::OSInfo::Start();
            
            // initialize window manager
            WindowMgr.Initialize();                       

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

            WindowMgr.Update();
        }

        // draw
        char str[32];
        void XServerHost::Draw()
        {
            // clear the screen
            FullCanvas.Clear(BackColor);

            // draw status panel
            uint32_t spx = KernelIO::VESA.GetWidth() - 208;
            uint32_t spy = 8;
            FullCanvas.DrawFilledRectangle(spx, spy, 200, 64, ButtonStyle.Colors[0]);
            FullCanvas.DrawRectangle3D(spx, spy, 200, 64, ButtonStyle.Colors[2], ButtonStyle.Colors[3], ButtonStyle.Colors[4]);

            // status panel - fps
            FullCanvas.DrawString(spx + 5, spy + 5, "FPS: ", ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF);
            FullCanvas.DrawString(spx + 93, spy + 5, fpsString, ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF);

            // status panel - used mem
            strdec(mem_get_used() / 1024, str);
            FullCanvas.DrawString(spx + 5, spy + 16, "RAM USED: ", ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF);
            FullCanvas.DrawString(spx + 93, spy + 16, str, ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF);
            uint32_t sw = strlen(str) * 8;
            FullCanvas.DrawString(spx + 93 + sw, spy + 16, " KB", ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF);
            strdec(mem_get_total_usable() / 1024 / 1024, str);
            FullCanvas.DrawString(spx + 5, spy + 27, "RAM TOTAL: ", ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF);
            FullCanvas.DrawString(spx + 93, spy + 27, str, ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF);
            sw = strlen(str) * 8;
            FullCanvas.DrawString(spx + 93 + sw, spy + 27, " MB", ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF);

            // status panel - active window
            strdec((uint32_t)WindowMgr.ActiveWindow, str);
            FullCanvas.DrawString(spx + 5, spy + 38, "ACTIVE WIN: ", ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF);
            FullCanvas.DrawString(spx + 93, spy + 38, str, ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF);
            
            WindowMgr.Draw();

            // draw taskbar
            Taskbar.Draw();

            // draw mouse
            KernelIO::Mouse.Draw();

            // swap buffer
            FullCanvas.Display();
        }

        // check if xserver is running
        bool XServerHost::IsRunning() { return Running; }

        XServerTaskbarItem::XServerTaskbarItem() { }

        XServerTaskbarItem::XServerTaskbarItem(char* text)
        {
            Bounds = bounds_t();
            Bounds.X = 0;
            Bounds.Y = KernelIO::VESA.GetHeight() - 21;
            Bounds.Width = 100;
            Bounds.Height = 18;

            Name = text;
        }

        // initialize taskbar
        void XServerTaskbar::Initialize()
        {

        }

        // update taskbar
        void XServerTaskbar::Update()
        {
            // update taskbar items
            uint32_t xx = 64;
            uint32_t yy = KernelIO::VESA.GetHeight() - 21;

            // populate list - max allowed windows
            for (size_t i = 0; i < 32; i++)
            {
                Items[i] = NULL;
            }

            for (size_t i = 0; i < 4; i++)
            {
                if (KernelIO::XServer.WindowMgr.Windows[i] != nullptr)
                {
                    Items[i] = XServerTaskbarItem(KernelIO::XServer.WindowMgr.Windows[i]->Title);
                    Items[i].Window = KernelIO::XServer.WindowMgr.Windows[i];
                }
            }

            // update list
            for (size_t i = 0; i < 32; i++)
            {
                if (&Items[i] != nullptr && Items[i].Name != nullptr && strlen(Items[i].Name) > 0 && !streql(Items[i].Name, "\0"))
                {
                    Items[i].Bounds.X = xx;
                    Items[i].Bounds.Y = yy;
                    xx += Items[i].Bounds.Width + 4;

                    if (bounds_contains(&Items[i].Bounds, KernelIO::Mouse.GetX(), KernelIO::Mouse.GetY()))
                    {
                        if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Pressed)
                        {
                            Items[i].Down = true;
                            for (size_t j = 0; j < KernelIO::XServer.WindowMgr.Index + 1; j++)
                            {
                                if (KernelIO::XServer.WindowMgr.Windows[j] == (Window*)Items[i].Window)
                                {
                                    KernelIO::XServer.WindowMgr.ActiveWindow = (Window*)Items[i].Window;
                                    break;
                                }
                            }
                        }
                        else { Items[i].Down = false; }
                    }
                }
            }
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

            // draw items
            for (size_t i = 0; i < 4; i++)
            {
                if (&Items[i] != nullptr && Items[i].Name != nullptr && strlen(Items[i].Name) > 0 && !streql(Items[i].Name, "\0"))
                {
                    // draw border
                    if (KernelIO::XServer.WindowMgr.ActiveWindow == (Window*)Items[i].Window) 
                    { KernelIO::XServer.FullCanvas.DrawRectangle3D(Items[i].Bounds.X, Items[i].Bounds.Y, Items[i].Bounds.Width, Items[i].Bounds.Height, ButtonStyle.Colors[4], ButtonStyle.Colors[2], ButtonStyle.Colors[2]); }
                    else { KernelIO::XServer.FullCanvas.DrawRectangle3D(Items[i].Bounds.X, Items[i].Bounds.Y, Items[i].Bounds.Width, Items[i].Bounds.Height, ButtonStyle.Colors[2], ButtonStyle.Colors[3], ButtonStyle.Colors[4]); }

                    // draw name
                    KernelIO::XServer.FullCanvas.DrawString(Items[i].Bounds.X + 3, Items[i].Bounds.Y + 4, Items[i].Name, ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF);
                }
            }
        }
    }
}