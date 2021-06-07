#include <gui/xserver.hpp>
#include <core/kernel.hpp>
#include <apps/app_term.hpp>
#include <apps/app_taskmgr.hpp>
#include <apps/app_debug.hpp>

namespace System
{
    const char* wallpaper_path = "/sys/resources/aeros_wp.bmp";

    int32_t time, last_time, fps, frames;
    char fps_string[64];
    Graphics::Bitmap* ico_term, *ico_folder, *ico_blank, *ico_alert, *ico_notes, *ico_settings;
    Graphics::Bitmap* wallpaper;

    XServerHost::XServerHost()
    {
        Running = false;
    }

    void XServerHost::Start()
    {
        if (!Running)
        {   
            // initialize window manager
            KernelIO::WindowMgr.Initialize();

            // load icons
            ico_term = new Graphics::Bitmap("/sys/resources/term32.bmp");
            ico_folder = new Graphics::Bitmap("/sys/resources/folder32.bmp");
            ico_blank = new Graphics::Bitmap("/sys/resources/blank32.bmp");
            ico_alert = new Graphics::Bitmap("/sys/resources/alert32.bmp");
            ico_notes = new Graphics::Bitmap("/sys/resources/notes32.bmp");
            ico_settings = new Graphics::Bitmap("/sys/resources/settings32.bmp");

            // load wallpaper
            wallpaper = new Graphics::Bitmap((char*)wallpaper_path);
            AutoSizeWallpaper();

            // must be initialized after icons are loaded
            Taskbar.Initialize();
            Menu.Initialize();

            Applications::WinDebug* debug = new Applications::WinDebug(128, 128);
            KernelIO::WindowMgr.Start(debug);

            // set running flag
            Running = true;
        }
    }

    void XServerHost::Update()
    {
        // get framerate
        CalculateFPS();

        Taskbar.Update();
        Menu.Update();

        KernelIO::WindowMgr.Update();
    }

    void XServerHost::Draw()
    {
        if (!KernelIO::WindowMgr.IsAnyMaximized())
        {
            if (wallpaper->Depth != COLOR_DEPTH_32) { Graphics::Canvas::Clear({ 0xFF, 156, 27, 12 }); }
            else { Graphics::Canvas::DrawBitmapFast(0, 0, wallpaper); }
        }

        KernelIO::WindowMgr.Draw();

        Taskbar.Draw();
        Menu.Draw();

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

    // auto resize wallpaper based on screen size
    void XServerHost::AutoSizeWallpaper()
    {
        uint32_t w = KernelIO::VESA.GetWidth();
        uint32_t h = KernelIO::VESA.GetHeight();

        if (wallpaper->Width != w || wallpaper->Height != h)
        {
            Graphics::Bitmap* scaled_wallpaper = new Graphics::Bitmap((char*)wallpaper_path);
            scaled_wallpaper->Resize(w, h);
            if (wallpaper != nullptr) { delete wallpaper; }
            wallpaper = scaled_wallpaper;
        }
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

        // on menu button clicked
        if (BtnDown && !BtnClicked)
        {
            KernelIO::XServer.Menu.Visible = !KernelIO::XServer.Menu.Visible;
            BtnClicked = true;
        }
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

    XMenuItem::XMenuItem() { }
    
    XMenuItem::XMenuItem(char* text, Graphics::Bitmap* icon)
    {
        Text = text;
        Icon = icon;
        bounds_set(&Bounds, 0, 0, 48, 64);
    }

    void XMenu::Initialize()
    {
        Items = new XMenuItem*[64];

        // create default items
        ItemFiles = new XMenuItem("Files", ico_folder); AddItem(ItemFiles);
        ItemNotes = new XMenuItem("Notes", ico_notes);  AddItem(ItemNotes);
        ItemTerm = new XMenuItem("XTerm", ico_term);    AddItem(ItemTerm);
        ItemSettings = new XMenuItem("Setup", ico_settings);  AddItem(ItemSettings);
    }

    void XMenu::Update()
    {
        int xx = Bounds.X + 8, yy = Bounds.Y + 8;
        int ww = 72, hh = 64;

        // set bounds
        bounds_set(&Bounds, KernelIO::XServer.Taskbar.Bounds.X, KernelIO::XServer.Taskbar.Bounds.Y + KernelIO::XServer.Taskbar.Bounds.Height, 264, 200);

        // update items
        if (Visible)
        {
            for (size_t i = 0; i < ItemCount; i++)
            {
                if (Items[i] != nullptr)
                {
                    bounds_set(&Items[i]->Bounds, xx, yy, ww, hh);

                    if (bounds_contains(&Items[i]->Bounds, KernelIO::Mouse.GetX(), KernelIO::Mouse.GetY()))
                    {
                        Items[i]->Hover = true;
                        if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Pressed) { Items[i]->Down = true; }
                        else { Items[i]->Down = false; Items[i]->Clicked = false; }
                    }
                    else { Items[i]->Hover = false; Items[i]->Down = false; Items[i]->Clicked = false; }

                    xx += ww + 8;
                    if (xx >= Bounds.Width - ww)
                    {
                        xx = Bounds.X + 8;
                        yy += hh + 8;
                    }
                }
            }
            // check for default item clicks
            if (ItemTerm->Down && !ItemTerm->Clicked)
            {
                KernelIO::WindowMgr.Start(new Applications::WinTerminal(163, 163));
                Visible = false;
                ItemTerm->Clicked = true;
            }
        }
    }

    void XMenu::Draw()
    {
        if (Visible)
        {
            Graphics::Canvas::DrawFilledRectangle(Bounds, KernelIO::XServer.Taskbar.Style.Colors[0]);
            Graphics::Canvas::DrawRectangle(Bounds, 1, GUI::WindowStyle.Colors[2]);

            // draw items
            for (size_t i = 0; i < ItemCount; i++)
            {
                if (Items[i] != nullptr)
                {
                    int icon_x = Items[i]->Bounds.X + ((Items[i]->Bounds.Width / 2) - 16);
                    int icon_y = Items[i]->Bounds.Y + 8;

                    // draw hover box
                    if (Items[i]->Hover) { Graphics::Canvas::DrawFilledRectangle(icon_x - 3, icon_y - 3, 38, 38, GUI::WindowStyle.Colors[3]); }

                    // draw icon
                    if (Items[i]->Icon != nullptr)
                    {
                        if (Items[i]->Icon->Depth == COLOR_DEPTH_32) { Graphics::Canvas::DrawBitmap(icon_x, icon_y, Graphics::Colors::Magenta, Items[i]->Icon); }
                    }

                    // draw text
                    if (Items[i]->Text != nullptr)
                    {
                        if (strlen(Items[i]->Text) > 0)
                        {
                            int tx = (Items[i]->Bounds.X - 4) + (((Items[i]->Bounds.Width + 8) / 2) - (strlen(Items[i]->Text) * 4));
                            int ty = Items[i]->Bounds.Y + 48;
                            Graphics::Canvas::DrawString(tx, ty, Items[i]->Text, GUI::WindowStyle.Colors[4], Graphics::FONT_8x8_SERIF);
                        }
                    }
                }
            }
        }
    }
    
    void XMenu::AddItem(XMenuItem* item)
    {
        if (item == nullptr) { return; }
        Items[ItemIndex] = item;
        ItemIndex++;
        ItemCount++;
    }

    void XMenu::AddItem(char* text, Graphics::Bitmap* icon)
    {
        XMenuItem* item = new XMenuItem();
        item->Text = text;
        item->Icon = icon;
        Items[ItemIndex] = item;
        ItemIndex++;
        ItemCount++; 
    }
}