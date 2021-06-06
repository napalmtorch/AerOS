#include <apps/app_taskmgr.hpp>
#include <core/kernel.hpp>

namespace System
{
    namespace Applications
    {
        WinTaskManager::WinTaskManager() : GUI::Window()
        {

        }

        WinTaskManager::WinTaskManager(int32_t x, int32_t y) : GUI::Window(x, y, 320, 400, "Task Manager", "taskmgr")
        {

        }

        WinTaskManager::~WinTaskManager()
        {

        }

        void WinTaskManager::OnLoad()
        {
            GUI::Window::OnLoad();
        }

        void WinTaskManager::Update()
        {
            GUI::Window::Update();
        }

        void WinTaskManager::Draw()
        {
            GUI::Window::Draw();
        }
    }
}