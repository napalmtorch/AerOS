#include <gui/winmgr.hpp>

namespace System
{
    namespace GUI
    {
        // window manager
        namespace WindowManager
        {
            System::List<Window*> Windows;
            
            void Initialize()
            {
                // initialize window list
                Windows = List<Window*>();
            }   

            void Update()
            {   
                // update windows
                for (size_t i = 0; i < Windows.Count; i++)
                {
                    (*Windows.Get(i))->Update();
                }
            }

            void Draw()
            {
                // draw windows
                for (size_t i = 0; i < Windows.Count; i++)
                {
                    (*Windows.Get(i))->Draw();
                }
            }

            void Start(Window* win)
            {
                // check if window is already open
                for (size_t i = 0; i < Windows.Count; i++)
                {
                    if ((*Windows.Get(i)) == win) { KernelIO::ThrowError("Window already open"); return; }
                }

                Windows.Add(win);
            }

            void Close(Window* win)
            {
                // check if window is already open
                bool open = false;
                uint32_t open_index = 0;
                for (size_t i = 0; i < Windows.Count; i++)
                {
                    if ((*Windows.Get(i)) == win) { open = true; open_index = i; break; }
                }
                if (!open) { KernelIO::ThrowError("Tried to close open window"); return; }

                Windows.RemoveAt(open_index);
            }
        }
    }
}