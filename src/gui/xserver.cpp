#include "gui/xserver.hpp"
#include <core/kernel.hpp>
#include <gui/widget.hpp>
#include <apps/win_term.hpp>
#include <apps/win_tview.hpp>
#include <apps/win_raycast.hpp>
#include <apps/win_bmp.hpp>
#include <graphics/colors.hpp>
#include <graphics/canvas.hpp>

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

        // wallpapers
        Graphics::Bitmap* Wallpaper;

        // performance stats
        char fpsString[16];
        char winCountStr[16];

        // initialize xserver interface
        void XServerHost::Initialize()
        {
            Running = true;
            FPS = 0;
            time = 1;
            fpsString[0] = '0';
            fpsString[1] = '\0';
            BackColor = Graphics::Colors::DarkCyan;
        }

        // start
        void XServerHost::Start()
        {
            Initialize();

            // set driver
            KernelIO::ThrowOK("Set graphics driver to VESA");
            
            // setup mouse
            KernelIO::Mouse.SetBounds(0, 0, KernelIO::VESA.GetWidth(), KernelIO::VESA.GetHeight());
            KernelIO::Mouse.SetPosition(KernelIO::VESA.GetWidth() / 2, KernelIO::VESA.GetHeight() / 2);
            KernelIO::ThrowOK("Set mouse bounds");

            // initialize taskbar and menu
            Taskbar.Initialize();
            Menu.Initialize();
            Menu.Visible = false;
            Menu.SelectedIndex = -1;
            KernelIO::ThrowOK("Initialized desktop");;

            MenuBtn = Button(4, KernelIO::VESA.GetHeight() - 21, "");
            MenuBtn.SetSize(56, 18);
            
            // initialize window manager
            WindowMgr.Initialize();        
            KernelIO::ThrowOK("Initialized window manager");               

            //if (Wallpaper == nullptr) { Wallpaper = new Graphics::Bitmap("/test/bg.bmp");}

            // running flag
            Running = true;
        }

        // update
        bool item_down = false;
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

            // update taskbar and menu
            Taskbar.Update();
            MenuBtn.Update();
            Menu.Update();

            // start button clicked
            if (MenuBtn.MouseFlags->Down && !MenuBtn.MouseFlags->Clicked)
            {
                Menu.Visible = !Menu.Visible;
                MenuBtn.MouseFlags->Clicked = true;
            }

            // menu items clicked
            if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Pressed && !item_down)
            {
                if (Menu.SelectedIndex == 0) { WindowMgr.Open(new Applications::WinBitmapViewer(64, 64)); Menu.Visible = false; }
                // text viewer
                if (Menu.SelectedIndex == 1) { WindowMgr.Open(new Applications::WinTextViewer(64, 64)); Menu.Visible = false; }
                // raycaster
                if (Menu.SelectedIndex == 2) { WindowMgr.Open(new Applications::WinRaycaster(64, 64)); Menu.Visible = false; }
                // terminal
                if (Menu.SelectedIndex == 3) { WindowMgr.Open(new Applications::WinTerminal(64, 64)); Menu.Visible = false; }
                // reboot
                if (Menu.SelectedIndex == 4) { KernelIO::Shell.ParseCommand("POWEROFF"); Menu.Visible = false; }
                // shutdown
                if (Menu.SelectedIndex == 5) { KernelIO::Shell.ParseCommand("REBOOT"); Menu.Visible = false; }
                
                item_down = true;
            }

            if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Released) { item_down = false; }

            WindowMgr.Update();
        }

        // draw
        char str[32];
        void XServerHost::Draw()
        {
            // clear the screen
            if (Wallpaper == nullptr) { Graphics::Canvas::Clear(BackColor); }
            else { Graphics::Canvas::DrawBitmap(-2, -2, 2, Wallpaper); }
            
            // draw status panel
            uint32_t spx = KernelIO::VESA.GetWidth() - 208;
            uint32_t spy = 8;
            Graphics::Canvas::DrawFilledRectangle(spx, spy, 200, 64, ButtonStyle.Colors[0]);
            Graphics::Canvas::DrawRectangle3D(spx, spy, 200, 64, ButtonStyle.Colors[2], ButtonStyle.Colors[3], ButtonStyle.Colors[4]);

            // status panel - fps
            Graphics::Canvas::DrawString(spx + 5, spy + 5, "FPS: ", ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF);
            Graphics::Canvas::DrawString(spx + 93, spy + 5, fpsString, ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF);

            // status panel - used mem
            strdec(mem_get_used() / 1024, str);
            Graphics::Canvas::DrawString(spx + 5, spy + 16, "RAM USED: ", ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF);
            Graphics::Canvas::DrawString(spx + 93, spy + 16, str, ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF);
            uint32_t sw = strlen(str) * 8;
            Graphics::Canvas::DrawString(spx + 93 + sw, spy + 16, " KB", ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF);
            strdec(mem_get_total_usable() / 1024 / 1024, str);
            Graphics::Canvas::DrawString(spx + 5, spy + 27, "RAM TOTAL: ", ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF);
            Graphics::Canvas::DrawString(spx + 93, spy + 27, str, ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF);
            sw = strlen(str) * 8;
            Graphics::Canvas::DrawString(spx + 93 + sw, spy + 27, " MB", ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF);

            // status panel - active window
            strdec((uint32_t)WindowMgr.ActiveWindow, str);
            Graphics::Canvas::DrawString(spx + 5, spy + 38, "ACTIVE WIN: ", ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF);
            Graphics::Canvas::DrawString(spx + 93, spy + 38, str, ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF);
            
            WindowMgr.Draw();
            
            // draw taskbar and menu
            Taskbar.Draw();
            MenuBtn.Draw();
            Menu.Draw();

            // draw menu visible button stuff
            Graphics::Canvas::DrawString(MenuBtn.Bounds->X + 7, MenuBtn.Bounds->Y + 5, "AerOS", ButtonStyle.Colors[1], Graphics::FONT_8x8_PHEONIX);
            if (Menu.Visible) { Graphics::Canvas::DrawRectangle3D(MenuBtn.Bounds->X, MenuBtn.Bounds->Y, MenuBtn.Bounds->Width, MenuBtn.Bounds->Height, ButtonStyle.Colors[4], ButtonStyle.Colors[2], ButtonStyle.Colors[2]); }

            

            // draw mouse
            KernelIO::Mouse.Draw();
            KernelIO::VESA.Render();

            // swap buffer
            //Canvas::Display();
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
            Graphics::Canvas::DrawFilledRectangle(0, KernelIO::VESA.GetHeight() - 24, KernelIO::VESA.GetWidth(), 24, ButtonStyle.Colors[0]);

            // draw border
            Graphics::Canvas::DrawFilledRectangle(0, KernelIO::VESA.GetHeight() - 23, KernelIO::VESA.GetWidth(), 1, ButtonStyle.Colors[2]);

            // draw tray
            Graphics::Canvas::DrawRectangle3D(KernelIO::VESA.GetWidth() - 100, KernelIO::VESA.GetHeight() - 21, 97, 18, ButtonStyle.Colors[3], ButtonStyle.Colors[2], ButtonStyle.Colors[2]);

            // draw time
            char* time = KernelIO::RTC.GetTimeString(false, false);
            Graphics::Canvas::DrawString(KernelIO::VESA.GetWidth() - (strlen(time) * 8) - 4, KernelIO::VESA.GetHeight() - 16, time, ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF);

            // draw items
            for (size_t i = 0; i < 4; i++)
            {
                if (&Items[i] != nullptr && Items[i].Name != nullptr && strlen(Items[i].Name) > 0 && !streql(Items[i].Name, "\0"))
                {
                    // draw border
                    if (KernelIO::XServer.WindowMgr.ActiveWindow == (Window*)Items[i].Window) 
                    { Graphics::Canvas::DrawRectangle3D(Items[i].Bounds.X, Items[i].Bounds.Y, Items[i].Bounds.Width, Items[i].Bounds.Height, ButtonStyle.Colors[4], ButtonStyle.Colors[2], ButtonStyle.Colors[2]); }
                    else { Graphics::Canvas::DrawRectangle3D(Items[i].Bounds.X, Items[i].Bounds.Y, Items[i].Bounds.Width, Items[i].Bounds.Height, ButtonStyle.Colors[2], ButtonStyle.Colors[3], ButtonStyle.Colors[4]); }

                    // draw name
                    Graphics::Canvas::DrawString(Items[i].Bounds.X + 3, Items[i].Bounds.Y + 4, Items[i].Name, ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF);
                }
            }
        }

        // initialize menu
        void XServerMenu::Initialize()
        {
            for (size_t i = 0; i < 16; i++) { Items[i] = NULL; }
            Items[Count++] = XServerTaskbarItem("Bitmap Viewer");
            Items[Count++] = XServerTaskbarItem("Text Viewer");
            Items[Count++] = XServerTaskbarItem("Raycaster");
            Items[Count++] = XServerTaskbarItem("Terminal");
            Items[Count++] = XServerTaskbarItem("Reboot");
            Items[Count++] = XServerTaskbarItem("Shutdown");
        }

        // update menu
        void XServerMenu::Update()
        {
            if (Count == 0) { return; }

            // update bounds
            Bounds.X = 0;
            Bounds.Y = KernelIO::VESA.GetHeight() - 24 - ((Count * 20) + 3);
            Bounds.Width = 128;
            Bounds.Height = (Count * 20) + 3;

            // update items
            uint32_t xx = Bounds.X + 1, yy = Bounds.Y + 1;
            int32_t sel = -1;
            for (size_t i = 0; i < 16; i++)
            {
                if (&Items[i] != nullptr)
                {
                    Items[i].Bounds.X = xx;
                    Items[i].Bounds.Y = yy;
                    Items[i].Bounds.Width = Bounds.Width - 3;
                    Items[i].Bounds.Height = 20;
                    if (bounds_contains(&Items[i].Bounds, KernelIO::Mouse.GetX(), KernelIO::Mouse.GetY()))
                    {
                        sel = i;
                        Items[i].Hover = true;
                        if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Pressed) { Items[i].Down = true; }
                        else { Items[i].Down = false; }
                    }
                    else { Items[i].Hover = false; }

                    yy += 20;
                }
            }

            SelectedIndex = sel;
        }

        // drwa menu
        void XServerMenu::Draw()
        {
            if (Count == 0) { return; }
            if (!Visible) { return; }

            Graphics::Canvas::DrawFilledRectangle(Bounds, ButtonStyle.Colors[0]);

            Graphics::Canvas::DrawRectangle3D(Bounds.X, Bounds.Y, Bounds.Width, Bounds.Height, ButtonStyle.Colors[2], ButtonStyle.Colors[3], ButtonStyle.Colors[4]);

            for (size_t i = 0; i < 16; i++)
            {
                if (&Items[i] != nullptr)
                {
                    if (Items[i].Name != nullptr && strlen(Items[i].Name) > 0 && !streql(Items[i].Name, "\0"))
                    {
                        if (Items[i].Hover)
                        {
                            Graphics::Canvas::DrawFilledRectangle(Items[i].Bounds, WindowStyle.Colors[5]);
                            Graphics::Canvas::DrawString(Items[i].Bounds.X + 5, Items[i].Bounds.Y + 6, Items[i].Name, WindowStyle.Colors[6], Graphics::FONT_8x8_SERIF);
                        }
                        else
                        { Graphics::Canvas::DrawString(Items[i].Bounds.X + 5, Items[i].Bounds.Y + 6, Items[i].Name, ButtonStyle.Colors[1], Graphics::FONT_8x8_SERIF); }
                    }
                }
            }
        }
    }
}