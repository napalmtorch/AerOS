#pragma once

#include <hardware/drivers/peripherals/ps2.hpp>
#include <hardware/interrupt/isr.hpp>
// Mouse-specific commands
#define MOUSE_SET_SAMPLE 0xF3
#define MOUSE_SET_RESOLUTION 0xE8
#define MOUSE_ENABLE_SCALING 0xE7
#define MOUSE_DISABLE_SCALING 0xE7

// Bits of the first mouse data packet
#define MOUSE_Y_OVERFLOW (1 << 7)
#define MOUSE_X_OVERFLOW (1 << 6)
#define MOUSE_Y_NEG (1 << 5)
#define MOUSE_X_NEG (1 << 4)
#define MOUSE_ALWAYS_SET (1 << 3)
#define MOUSE_MIDDLE (1 << 2)
#define MOUSE_RIGHT (1 << 1)
#define MOUSE_LEFT (1 << 0)

// Bits of the fourth mouse data packet
#define MOUSE_UNUSED_A (1 << 7)
#define MOUSE_UNUSED_B (1 << 6)

typedef struct {
    float x, y;
    bool left_pressed;
    bool right_pressed;
    bool middle_pressed;
} mouse_t;

typedef void (*mouse_callback_t)(mouse_t);

void init_mouse(uint32_t dev);
void mouse_handle_packet();
void mouse_handle_interrupt(registers_t* regs);
void mouse_set_callback(mouse_callback_t cb);

void mouse_set_sample_rate(uint8_t rate);
void mouse_set_resolution(uint8_t level);
void mouse_set_scaling(bool enabled);
void mouse_enable_scroll_wheel();
void mouse_enable_five_buttons();

int mouse_get_x();
int mouse_get_y();
bool mouse_left_button();
bool mouse_right_button();