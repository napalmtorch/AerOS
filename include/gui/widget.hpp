#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <graphics/canvas.hpp>
#include <graphics/colors.hpp>
#include <graphics/font.hpp>

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

// border style
typedef enum
{
    BORDER_STYLE_NONE,
    BORDER_STYLE_FIXED,
    BORDER_STYLE_3D,
} BORDER_STYLE;

// dialog result
typedef enum
{
    DIALOG_RESULT_OK,
    DIALOG_RESULT_NO,
    DIALOG_RESULT_YES,
    DIALOG_RESULT_CANCEL,
    DIALOG_RESULT_PENDING,
} DIALOG_RESULT;


namespace System
{
    namespace GUI
    {
        // title bar icons
        extern uint8_t TitleBarIcon_Exit[];
        extern uint8_t TitleBarIcon_Max[];

        extern uint32_t CheckMarkIcon[];

        // mouse flags
        typedef struct
        {
            bool Hover, Clicked;
            bool Down, Up;      
        } __attribute__((packed)) MouseEventFlags;

        // visual style structure
        typedef struct
        {
            // name
            char Name[32];
            // colors
            Color Colors[8];
            // border
            BORDER_STYLE BorderStyle;
            uint32_t BorderSize;
            // font
            Graphics::Font* Font;

        } VisualStyle;

        // default visual styles
        extern VisualStyle ButtonStyle;
        extern VisualStyle WindowStyle;
        extern VisualStyle CheckBoxStyle;

        typedef struct
        {
            bool Active;
            bool Visible;
            bool Maximized;
            bool Minimized;
            bool Moving;
            bool Resizing;
            bool Running;
            bool CanDraw;
        } __attribute__((packed)) WindowFlags;

        typedef struct
        {
            bool Active;
            bool Visible;
            bool Enabled;
            bool Toggled;
        } __attribute((packed)) WidgetFlags;
        
        extern uint8_t TITLEBAR_ICON_CLOSE[];
        extern uint8_t TITLEBAR_ICON_MAX[];
        extern uint8_t TITLEBAR_ICON_RES[];
        extern uint8_t TITLEBAR_ICON_MIN[];

        class Widget
        {
            // functions
            public:
                Widget();
                Widget(int32_t x, int32_t y, int32_t w, int32_t h, WIDGET_TYPE type);
                virtual void Update();
                virtual void Draw();

            // setters
            public:
                void SetPosition(int32_t x, int32_t y);
                void SetPosition(point_t* pos);
                void SetSize(int32_t w, int32_t h);
                void SetSize(point_t* size);
                void SetBounds(int32_t x, int32_t y, int32_t w, int32_t h);
                void SetBounds(point_t* pos, point_t* size);
                void SetBounds(bounds_t* bounds);
                void SetText(char* text);

            // events
            public:
                void (*OnClick)(void);
            
            // variables
            public:
                char* Text;
                WIDGET_TYPE Type;
                bounds_t* Bounds;
                MouseEventFlags* MouseFlags;
                VisualStyle* Style;
                WidgetFlags* Flags;
        };

        // button - derived from widget class
        class Button : public Widget
        {
            public:
                Button();
                Button(int32_t x, int32_t y, char* text);
                void Update() override;
                void Draw() override;
        };

        // check box - derived from widget class
        class CheckBox : public Widget
        {
            public:
                CheckBox();
                CheckBox(int32_t x, int32_t y, char* text);
                CheckBox(int32_t x, int32_t y, char* text, bool toggled);
                void Update() override;
                void Draw() override;
                bounds_t* CheckBounds;
            private:
                bool m_down;
        };

        // window - forward declaration
        class Window;

        // title bar - derived from widget class
        class TitleBar : public Widget
        {
            public:
                TitleBar();
                TitleBar(Window* window);
                void Update() override;
                void Draw() override;
                Window* Parent;
                Button* BtnClose;
                Button* BtnMax;
                Button* BtnMin;
        };

        class Window
        {
            // functions
            public:
                Window();
                Window(int32_t x, int32_t y, int32_t w, int32_t h, char* title);
                virtual void Update();
                virtual void Draw();

            // variables
            public:
                char* Title;
                char* Name;
                bounds_t* Bounds;
                bounds_t* ClientBounds;
                bounds_t* OldBounds;
                WindowFlags* Flags;
                VisualStyle* Style;
                TitleBar* TBar;
            
            private:
                void SetName(char* text);
        };

        VisualStyle* CopyStyle(VisualStyle* src);

        // check widget default events
        void CheckWidgetEvents(Widget* widget);

        // check if widget is null
        bool IsWidgetNull(Widget* widget);

        // set widget dimensions
        void SetWidgetPosition(Widget* widget, int32_t x, int32_t y);
        void SetWidgetSize(Widget* widget, int32_t w, int32_t h);
        void SetWidgetBounds(Widget* widget, int32_t x, int32_t y, int32_t w, int32_t h);
        
        // set properties
        void SetWidgetVisible(Widget* widget, bool visible);
        void SetWidgetEnabled(Widget* widget, bool enabled);
        void SetWidgetText(Widget* widget, char* text);   
    }
}