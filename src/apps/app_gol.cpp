#include <apps/app_gol.hpp>
#include <core/kernel.hpp>

namespace System
{
    namespace Applications
    {
        WinGameOfLife::WinGameOfLife() : GUI::Window()
        {

        }

        WinGameOfLife::WinGameOfLife(int x, int y) : GUI::Window(x, y, 320, 240, "Game of Life", "gol")
        {

        }

        WinGameOfLife::~WinGameOfLife()
        {

        }

        void update_cells()
        {
            while (true)
            {
                if (streql("gol", KernelIO::WindowMgr.GetWindow(KernelIO::WindowMgr.ActiveIndex)->GetID()))
                {
                    WinGameOfLife* game = ((WinGameOfLife*)KernelIO::WindowMgr.GetWindow(KernelIO::WindowMgr.ActiveIndex));
                    game->UpdateCells();
                }
            }
        }

        void WinGameOfLife::OnLoad()
        {
            GUI::Window::OnLoad();

            GridWidth = ClientBounds.Width - 1;
            GridHeight = ClientBounds.Height - 1;

            Cells = new uint8_t[GridWidth * GridHeight];
            mem_fill(Cells, 0, GridWidth * GridHeight);
            
            TempCells = new uint8_t[GridWidth * GridHeight];
            mem_fill(TempCells, 0, GridWidth * GridHeight);

            Image = new Graphics::Bitmap(GridWidth, GridHeight, COLOR_DEPTH_32);
            Generating = false;
        }

        void WinGameOfLife::StartThread()
        {
            //GameThread = new Threading::Thread("Game of Life", "gol", System::Threading::Priority::High, [] () { update_cells(); });
            //GameThread->Start();
        }

        void WinGameOfLife::OnClose()
        {
            GUI::Window::OnClose();
        }

        void WinGameOfLife::Update()
        {
            GUI::Window::Update();

            UpdateCells();

            // copy data to bitmap
            for (int i = 0; i < GridWidth * GridHeight; i++)
            {
                if (Cells[i] == 1) { Image->SetPixel(i % GridWidth, i / GridWidth, Graphics::Colors::White); }
                else { Image->SetPixel(i % GridWidth, i / GridWidth, Graphics::Colors::Black); }
            }

            int mx = KernelIO::Mouse.GetX() - (Bounds.X + ClientBounds.X);
            int my = KernelIO::Mouse.GetY() - (Bounds.Y + ClientBounds.Y);

            if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Pressed)
            {
                if (mx >= 0 && mx < GridWidth && my >= 0 && my < GridHeight)
                {
                    Cells[PointToIndex(mx, my)] = 1;
                }
            }

            if (KernelIO::Keyboard.IsKeyDown(HAL::Keys::SPACE) && !SpaceDown)
            {
                Generating = !Generating;
                SpaceDown = true;
            }
            if (KernelIO::Keyboard.IsKeyUp(HAL::Keys::SPACE)) { SpaceDown = false; }
        }

        void WinGameOfLife::Draw()
        {
            GUI::Window::Draw();
        }

        void WinGameOfLife::Refresh() 
        { 
            GUI::Window::Refresh(); 
            if (!Flags.Moving) { Graphics::Canvas::DrawBitmapFast(Bounds.X + ClientBounds.X, Bounds.Y + ClientBounds.Y, Image); }
        }

        void WinGameOfLife::UpdateCells()
        {
            if (Generating)
            {
                for (int i = 0; i < GridWidth * GridHeight; i++)
                {
                    ModifyCell(i % GridWidth, i / GridWidth);
                }

                mem_copy(TempCells, Cells, GridWidth * GridHeight);
            }
        }

        void WinGameOfLife::ModifyCell(int x, int y)
        {
            int neighbours = 0;
            int i = x + (y * GridWidth);
            uint8_t state = Cells[i];

            neighbours += Cells[PointToIndex(x - 1, y - 1)];
            neighbours += Cells[PointToIndex(x + 0, y - 1)];
            neighbours += Cells[PointToIndex(x + 1, y - 1)];
            neighbours += Cells[PointToIndex(x - 1, y + 0)];
            neighbours += Cells[PointToIndex(x + 1, y + 0)];
            neighbours += Cells[PointToIndex(x - 1, y + 1)];
            neighbours += Cells[PointToIndex(x + 0, y + 1)];
            neighbours += Cells[PointToIndex(x + 1, y + 1)];

            if (state == 0 && neighbours == 3) { TempCells[i] = 1; return; }
            if (state > 0 && (neighbours < 2 || neighbours > 3)) { TempCells[i] = 0; return; }
            TempCells[i] = state;
        }

        int WinGameOfLife::PointToIndex(int x, int y)
        {
            if (x < 0 || y < 0 || x >= GridWidth || y >= GridHeight) { return 0; }
            return x + (y * GridWidth);
        }
    }
}