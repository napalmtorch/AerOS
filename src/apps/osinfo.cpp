#include "apps/osinfo.hpp"
#include <core/kernel.hpp>

namespace Applications
{
    namespace OSInfo
    {
        System::GUI::Window* Window = nullptr;

        void Start()
        {
            Window = new System::GUI::Window(200, 200, 320, 240, "About AerOS");
            Window->ChildUpdate = Update;
            Window->ChildDraw = Draw;
            System::GUI::WindowManager::Start(Window);
        }

        void Update()
        {
            
        }

        void Draw()
        {
            System::KernelIO::XServer.FullCanvas.DrawFilledRectangle(Window->Base.Bounds.X + 4, Window->Base.Bounds.Y + 32, 32, 32, Graphics::Colors::Red);
        }
    }
}