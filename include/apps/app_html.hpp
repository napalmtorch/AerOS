#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <gui/widget.hpp>
#include <gui/xserver.hpp>
#include <software/html_parse.hpp>
namespace System
{
    namespace Applications
    {
        class WinHtml : public GUI::Window
        {
            // main methods
            public:
                WinHtml();
                WinHtml(int32_t x, int32_t y, char* title);
                ~WinHtml();
                void OnLoad() override;
                void Update() override;
                void Draw()   override;
        };
    }
}