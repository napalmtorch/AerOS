#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <graphics/canvas.hpp>
#include <graphics/colors.hpp>
#include <graphics/font.hpp>
#include <gui/xserver.hpp>

typedef enum
{
    WIDGET_TYPE_BUTTON,
    WIDGET_TYPE_TEXTBOX,
    WIDGET_TYPE_CHECKBOX,
    WIDGET_TYPE_LISTBOX,
    WIDGET_TYPE_PROGRESSBAR,
    WIDGET_TYPE_NUMERICUPDOWN,
    WIDGET_TYPE_CONTAINER,
    WIDGET_TYPE_WINDOW,
    WIDGET_TYPE_SYSTEM,
} WIDGET_TYPE;

namespace System
{
    namespace GUI
    {
        
    }
}