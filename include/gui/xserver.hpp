#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <graphics/canvas.hpp>
#include <gui/widget.hpp>

namespace System
{
    struct XMenuItem
    {
        char* Text;
        Graphics::Bitmap* Icon;
        bounds_t Bounds;
        bool Hover;
        bool Down;
        bool Clicked;
        XMenuItem();
        XMenuItem(char* text, Graphics::Bitmap* icon);
    };
    
    class XTaskbar
    {
        public:
            void Initialize();
            void Update();
            void Draw();

        public:
            bounds_t Bounds;
            GUI::VisualStyle Style;

        private:
            bounds_t BtnBounds;
            bool     BtnHover;
            bool     BtnDown;
            bool     BtnClicked;
    };

    class XMenu
    {
        public:
            void Initialize();
            void Update();
            void Draw();
            void AddItem(XMenuItem* item);
            void AddItem(char* text, Graphics::Bitmap* icon);
        
        public:
            bounds_t    Bounds;
            bool        Visible;
            XMenuItem** Items;
            uint32_t    ItemCount;
            int32_t     ItemIndex;

        // default items
        private:
            XMenuItem* ItemFiles;
            XMenuItem* ItemNotes;
            XMenuItem* ItemTerm;
            XMenuItem* ItemTasks;
            XMenuItem* ItemSettings;

    };

    class XServerHost
    {
        public:
            XServerHost();
            void Start();
            void Update();
            void Draw();
            void OnInterrupt();
            void AutoSizeWallpaper();
            bool IsRunning();
            int32_t GetFPS();
            char* GetFPSString();

        private:
            void CalculateFPS();

        public:
            XTaskbar Taskbar;
            XMenu Menu;

        private:
            bool Running;
    };
}