#include <gui/widget.hpp>
#include <core/kernel.hpp>

namespace System
{
    namespace GUI
    {
        // constructor - blank
        Window::Window() { }

        // constructor - create window with specified properties
        Window::Window(int32_t x, int32_t y, int32_t w, int32_t h, char* title, char* name)
        {
            Title = title;
            Name = name;

            Flags.CanDraw = true;
            Flags.CanMaximize = true;
            Flags.CanMinimize = true;
            MinimumSize = { w, h };
            MaximumSize = { (int32_t)KernelIO::VESA.GetWidth(), (int32_t)KernelIO::VESA.GetHeight() };

            SetBounds(x, y, w, h);
            CopyStyle(&WindowStyle, &Style);

            // update client bounds
            UpdateClientBounds();
        }

        // destroy window
        Window::~Window() { }

        void Window::OnLoad()
        {
            
        }

        // update window
        bool move_click, resize_click_r, resize_click_b;
        int32_t mx_start, my_start;
        void Window::Update()
        {
            // update when window is active
            if (Flags.Active)
            {
                // update client bounds
                UpdateClientBounds();

                // update resize bounds
                bounds_set(&ResizeBoundsRight, Bounds.X + Bounds.Width - 3, Bounds.Y + 20, 3, Bounds.Height - 20);
                bounds_set(&ResizeBoundsBottom, Bounds.X, Bounds.Y + Bounds.Height - 3, Bounds.Width, 3);

                // update button bounds
                bounds_set(&CloseBounds, Bounds.X + Bounds.Width - 16, Bounds.Y + 6, 9, 9);
                bounds_set(&MaxBounds, Bounds.X + Bounds.Width - 32, Bounds.Y + 6, 9, 9);
                bounds_set(&MinBounds, Bounds.X + Bounds.Width - 48, Bounds.Y + 6, 9, 9);

                int32_t mx = KernelIO::Mouse.GetX(), my = KernelIO::Mouse.GetY();

                // check close button state
                if (bounds_contains(&CloseBounds, mx, my))
                {
                    CloseHover = true;
                    if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Pressed) { CloseDown = true; }
                    else { CloseDown = false; CloseClicked = false; }
                }
                else { CloseHover = false; CloseDown = false; CloseClicked = false; }

                // check max button state
                if (bounds_contains(&MaxBounds, mx, my))
                {
                    MaxHover = true;
                    if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Pressed) { MaxDown = true; }
                    else { MaxDown = false; MaxClicked = false; }
                }
                else { MaxHover = false; MaxDown = false; MaxClicked = false; }

                // check min button state
                if (bounds_contains(&MinBounds, mx, my))
                {
                    MinHover = true;
                    if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Pressed) { MinDown = true; }
                    else { MinDown = false; MinClicked = false; }
                }
                else { MinHover = false; MinDown = false; MinClicked = false; }

                // close button clicked
                if (CloseDown && !CloseClicked)
                {
                    Flags.ExitRequest = true;
                    CloseClicked = true;
                    return;
                }

                // max button clicked
                if (MaxDown && !MaxClicked)
                {
                    KernelIO::WindowMgr.SetActiveWindow(this);
                    if (Flags.Maximized)
                    {
                        bounds_set(&Bounds, OldBounds.X, OldBounds.Y, OldBounds.Width, OldBounds.Height);
                        Flags.Maximized = false;
                    }
                    else
                    {
                        bounds_set(&OldBounds, Bounds.X, Bounds.Y, Bounds.Width, Bounds.Height);
                        bounds_set(&Bounds, 0, 20, KernelIO::VESA.GetWidth(), KernelIO::VESA.GetHeight() - 20);
                        Flags.Maximized = true;
                    }

                    MaxClicked = true;
                }

                // min button down
                if (MinDown && !MinClicked)
                {
                    MinClicked = true;
                }

                // check for moving/sizing
                if (!Flags.Maximized && !Flags.Minimized)
                {
                    // title bar bounds
                    bounds_t bnds = { Bounds.X, Bounds.Y, Bounds.Width, 20 };

                    // check for movement
                    if (bounds_contains(&bnds, mx, my) && !CloseHover && !MaxHover && !MinHover && !Flags.Resizing)
                    {
                        if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Pressed)
                        {
                            if (!move_click)
                            {
                                mx_start = mx - Bounds.X;
                                my_start = my - Bounds.Y;
                                move_click = true;
                            }
                            Flags.Moving = true;
                        }
                    }

                    // check for right side resize
                    if (bounds_contains(&ResizeBoundsRight, mx, my) && !Flags.Moving)
                    {
                        if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Pressed)
                        {
                            if (!resize_click_r) { resize_click_r = true; }
                            Flags.Resizing = true;
                        }
                    }

                    // check for bottom size resize
                    if (bounds_contains(&ResizeBoundsBottom, mx, my) && !Flags.Moving)
                    {
                        if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Pressed)
                        {
                            if (!resize_click_b) { resize_click_b = true; }
                            Flags.Resizing = true;
                        }
                    }

                    // update cursor based on state
                    if (bounds_contains(&ResizeBoundsRight, mx, my)) { KernelIO::Mouse.SetCursor(HAL::CursorType::ResizeWE); }
                    if (bounds_contains(&ResizeBoundsBottom, mx, my)) { KernelIO::Mouse.SetCursor(HAL::CursorType::ResizeNS); }

                    // relocate window
                    if (Flags.Moving)
                    {
                        Bounds.X = mx - mx_start;
                        Bounds.Y = my - my_start;
                    }

                    // limit position ...
                    // finish this when the second taskbar/dock is done

                    // resize window
                    if (resize_click_r) { Bounds.Width = mx - Bounds.X; }
                    if (resize_click_b) { Bounds.Height = my - Bounds.Y; }

                    // limit size
                    if (Bounds.Width  < MinimumSize.X) { Bounds.Width  = MinimumSize.X; }
                    if (Bounds.Width  > MaximumSize.X) { Bounds.Width  = MaximumSize.X; }
                    if (Bounds.Height < MinimumSize.Y) { Bounds.Height = MinimumSize.Y; }
                    if (Bounds.Height > MaximumSize.Y) { Bounds.Height = MaximumSize.Y; }


                    // stopped dragging window
                    if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Released)
                    {
                        Flags.Moving = false;
                        Flags.Resizing = false;
                        move_click = false;
                        resize_click_r = false;
                        resize_click_b = false;
                    }

                    if (!resize_click_b && !resize_click_r && !bounds_contains(&ResizeBoundsBottom, mx, my) && !bounds_contains(&ResizeBoundsRight, mx, my)) 
                    { KernelIO::Mouse.SetCursor(HAL::CursorType::Default); }
                }
            }
            // window is not active
            else
            {
                Flags.Moving = false;
                Flags.Resizing = false;
            }

            Flags.CanDraw = (!Flags.Moving && !Flags.Minimized);
        }

        // draw window
        void Window::Draw()
        {
            if (Flags.CanDraw)
            {   
                // draw background
                if (!Flags.Maximized) { Graphics::Canvas::DrawFilledRectangle(ClientBounds, Style.Colors[0]); }
                else { Graphics::Canvas::Clear(Style.Colors[0]); }

                // draw title bar background
                Graphics::Canvas::DrawFilledRectangle(Bounds.X, Bounds.Y, Bounds.Width, 20, Style.Colors[3]);
                
                // draw border
                Graphics::Canvas::DrawRectangle(Bounds.X, Bounds.Y, Bounds.Width, Bounds.Height, 1, Style.Colors[2]);

                // draw close icon
                int close_i = 4;
                if (CloseHover) { close_i++; }
                Graphics::Canvas::DrawFlatArray(CloseBounds.X, CloseBounds.Y, CloseBounds.Width, CloseBounds.Height, (uint8_t*)ICON_TITLEBAR_EXIT_9x9, Style.Colors[close_i]);

                // draw maximize icon
                int max_i = 4;
                if (MaxHover) { max_i++; }
                Graphics::Canvas::DrawFlatArray(MaxBounds.X, MaxBounds.Y, MaxBounds.Width, MaxBounds.Height, (uint8_t*)ICON_TITLEBAR_MAX_9x9, Style.Colors[max_i]);

                // draw minimize icon
                int min_i = 4;
                if (MinHover) { min_i++; }
                Graphics::Canvas::DrawFilledRectangle(Bounds.X + Bounds.Width - 48, Bounds.Y + 13, 9, 2, Style.Colors[min_i]);

                // draw title
                Graphics::Canvas::DrawString(Bounds.X + 8, Bounds.Y + 6, Title, Style.Colors[4], Graphics::FONT_8x8);
            }
            else
            {
                // draw border
                Graphics::Canvas::DrawRectangle(Bounds, 2, Graphics::Colors::White);
                Graphics::Canvas::DrawRectangle(Bounds, 1, Graphics::Colors::Black);
            }
        }

        void Window::SetPosition(int32_t x, int32_t y) { Bounds.X = x; Bounds.Y = y; }
        void Window::SetSize(int32_t w, int32_t h) { Bounds.Width = w; Bounds.Height = h; }
        void Window::SetBounds(int32_t x, int32_t y, int32_t w, int32_t h) { SetPosition(x, y); SetSize(w, h); }

        void Window::UpdateClientBounds()
        {
            // update client bounds
            ClientBounds.X = Bounds.X + 1;
            ClientBounds.Y = Bounds.Y + 20;
            ClientBounds.Width = Bounds.Width - 2;
            ClientBounds.Height = Bounds.Height - 21;
        }
    }
}