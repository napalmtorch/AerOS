#pragma once
#include "lib/types.h"
#include "lib/types.h"
#include "lib/string.hpp"
#include "hardware/interrupt/isr.hpp"

namespace HAL
{
    enum class ButtonState
    {
        Released,
        Pressed,
    };

    struct MouseFlags
    {
        public:
            MouseFlags() { Hover = false; Down = false; Up = false; Clicked = false; }
            bool Hover;
            bool Down;
            bool Up;
            bool Clicked;
    };

    class PS2Mouse
    {
        public:
            // initialize mouse
            void Initialize();
            // handle mouse movement offsets
            void OnMouseMove(int8_t x, int8_t y);
            // on interrupt
            void OnInterrupt();
            // wait
            void Wait(uint8_t type);
            // write data
            void Write(uint8_t data);
            // read
            uint8_t Read();
            // buttons
            uint8_t GetButtons();
            ButtonState IsLeftPressed();
            ButtonState IsRightPressed();
            // position and bounds
            void SetBounds(int32_t x, int32_t y, int32_t w, int32_t h);
            void SetPosition(uint32_t x, uint32_t y);
            bounds_t GetBounds();
            point_t GetPosition();
            int32_t GetX();
            int32_t GetY();
        private:
            bounds_t Bounds;
            point_t Position;
            uint8_t Buffer[3];
            uint8_t Offset;
            uint8_t Buttons;
            uint8_t Cycle;
            ButtonState LeftPressed;
            ButtonState RightPressed;
    };
}