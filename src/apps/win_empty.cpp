#include "apps/win_empty.hpp"

namespace System
{
    namespace Applications
    {
        WinEmpty::WinEmpty()
        {

        }

        WinEmpty::WinEmpty(int32_t x, int32_t y) : GUI::Window(x, y, 320, 200, "Demo Window")
        {
            
        }

        void WinEmpty::Update()
        {
            GUI::Window::Update();
        }

        void WinEmpty::Draw()
        {
            GUI::Window::Draw();
        }
    }
}