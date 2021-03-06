#include "hardware/drivers/mouse.hpp"
#include "core/kernel.hpp"

void ms_callback(registers_t regs)
{
    System::KernelIO::Mouse.OnInterrupt();
    UNUSED(regs);
}

namespace HAL
{
    // constructor
    void PS2Mouse::Initialize()
    {
        // reset local properties
        this->Bounds.X = 0; this->Bounds.Y = 0;
        this->SetBounds(0, 0, 320, 200);
        this->Position.X = 0; this->Position.Y = 0;
        this->Offset = 0;
        this->Buttons = 0;
        this->LeftPressed = ButtonState::Released;
        this->RightPressed = ButtonState::Released;
        this->ArrowKeys = true;
        
        // setup controller
        outb(0x64, 0xA8);
        outb(0x64, 0x20);
        uint8_t status = inb(0x60) | 2;
        outb(0x64, 0x60);
        outb(0x60, status);
        outb(0x64, 0xD4);
        outb(0x60, 0xF4);
        inb(0x60);

        // register interrupt
        CPU::RegisterIRQ(IRQ12, (isr_t)ms_callback);

        SetCursor(CursorType::Default);
    }

    void PS2Mouse::SetCursor(CursorType cursor)
    {
        Cursor = cursor;
    }

    // draw mouse to screen
    void PS2Mouse::Draw()
    {
        switch (Cursor)
        {
            case CursorType::Default:  { Graphics::Canvas::DrawArray(Position.X, Position.Y, 12, 20, Graphics::Colors::Magenta, (uint32_t*)CursorData); break; }
            case CursorType::Loading:  { Graphics::Canvas::DrawArray(Position.X, Position.Y, 11, 20, Graphics::Colors::Magenta, (uint32_t*)CursorDataWait); break; }
            case CursorType::ResizeNS: { Graphics::Canvas::DrawArray(Position.X, Position.Y - 4, 7,  20,  Graphics::Colors::Magenta, (uint32_t*)CursorDataResizeNS); break; }
            case CursorType::ResizeWE: { Graphics::Canvas::DrawArray(Position.X - 10, Position.Y, 20, 7, Graphics::Colors::Magenta, (uint32_t*)CursorDataResizeWE); break; }
            default:                   { Graphics::Canvas::DrawArray(Position.X, Position.Y, 12, 20, Graphics::Colors::Magenta, (uint32_t*)CursorData); break; }
        }
    }

    // handle mouse movement offsets
    void PS2Mouse::OnMouseMove(int8_t x, int8_t y)
    {
        // increment pos
        Position.X += x;
        Position.Y += y;

        // clamp inside bounds
        if (Position.X < Bounds.X) { Position.X = 0; }
        if (Position.Y < Bounds.Y) { Position.Y = 0; }
        if (Position.X >= Bounds.Width) { Position.X = Bounds.Width; }
        if (Position.Y >= Bounds.Height) { Position.Y = Bounds.Height; }
    }

    // on interrupt
    void PS2Mouse::OnInterrupt()
    {
        uint8_t status = inb(0x64);
        if ((!(status & 1)) == 1) { Cycle = 0; return; }
        if ((!(status & 2)) == 0) { Cycle = 0; return; }
        if (!(status & 0x20)) { Cycle = 0; return; }

        switch (Cycle)
        {
            case 0:
            {
                Buffer[0] = inb(0x60);
                Cycle++;
                break;
            }
            case 1:
            {
                Buffer[1] = inb(0x60);
                Cycle++;
                break;
            }
            case 2:
            {
                Buffer[2] = inb(0x60);
                OnMouseMove(Buffer[1], -Buffer[2]);
                LeftPressed = (ButtonState)(Buffer[0] & 0b00000001);
                RightPressed = (ButtonState)((Buffer[0] & 0b00000010) >> 1);
                Cycle = 0;
                break;
            }
        }
    }

    void PS2Mouse::UpdateArrowKeyMovement()
    {
        /* not yet implemented */
    }

    // get buttons flag
    uint8_t PS2Mouse::GetButtons() { return Buttons; }

    // check if left mouse button is down
    ButtonState PS2Mouse::IsLeftPressed() { return LeftPressed; }

    // check if right mouse button is down
    ButtonState PS2Mouse::IsRightPressed() { return RightPressed; }

    // set boundary
    void PS2Mouse::SetBounds(int32_t x, int32_t y, int32_t w, int32_t h)
    {
        Bounds.X =  x;
        Bounds.Y = y;
        Bounds.Width = w - 1;
        Bounds.Height = h - 1;
    }

    // set position
    void PS2Mouse::SetPosition(uint32_t x, uint32_t y) { Position.X = x; Position.Y = y; }

    // get boundary
    bounds_t PS2Mouse::GetBounds() { return Bounds; }

    // get positions
    point_t PS2Mouse::GetPosition() { return Position; }

    // get x position
    int32_t PS2Mouse::GetX() { return Position.X; }

    // get y position
    int32_t PS2Mouse::GetY() { return Position.Y; }

    // get cursor image pixel array
    uint8_t* PS2Mouse::GetCursor() { return (uint8_t*)CursorData; }

    uint32_t* PS2Mouse::GetCursor32() { return (uint32_t*)CursorData; }

    void PS2Mouse::ToggleArrowKeys(bool value) { ArrowKeys = value; }

    bool PS2Mouse::GetArrowKeyState() { return ArrowKeys; }
}