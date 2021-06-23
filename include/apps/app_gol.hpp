#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <graphics/canvas.hpp>
#include <gui/window.hpp>
#include <lib/multitasking.h>

namespace System
{
    namespace Applications
    {
        class WinGameOfLife : public GUI::Window
        {
            public:
                WinGameOfLife();
                WinGameOfLife(int x, int y);
                ~WinGameOfLife();
                void OnLoad() override;
                void OnClose() override;
                void Update() override;
                void Draw() override;
                void Refresh() override;
                void StartThread();
                void UpdateCells();
                void ModifyCell(int x, int y);
                int PointToIndex(int x, int y);

            protected:
                Graphics::Bitmap* Image;
                uint32_t GridWidth;
                uint32_t GridHeight;
                uint32_t CellSize;
                uint8_t* Cells;
                uint8_t* TempCells;
                Threading::Thread* GameThread;
                bool Generating;
                bool SpaceDown;
        };
    }
}