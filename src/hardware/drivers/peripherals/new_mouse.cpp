#include <hardware/drivers/peripherals/mouse.hpp>
#include <hardware/interrupt/isr.hpp>

uint32_t current_byte = 0;
uint32_t bytes_per_packet = 3;
uint8_t packet[4] = { 0 };

uint32_t device;
mouse_t state;

mouse_callback_t callback;

/* Initializes a mouse plugged in controller `dev`.
 * Tries to enable as many of its features as possible.
 */
void init_mouse(uint32_t dev) {
    device = dev;

    isr_register(IRQ12, (isr_t)mouse_handle_interrupt);

    // Enable features
    mouse_enable_scroll_wheel();
    mouse_enable_five_buttons();

    // Set mouse parameters
    mouse_set_sample_rate(80);
    mouse_set_resolution(0x00); // One unit per mm
    mouse_set_scaling(false);   // Disable acceleration

    // Start receiving IRQ12s
    ps2_write_device(device, PS2_DEV_ENABLE_SCAN);
    ps2_expect_ack();
}

/* Receives one byte from the mouse and packages it in `packet`. Once that
 * packet is full, calls `mouse_handle_packet`.
 */
void mouse_handle_interrupt(registers_t* regs) {
    UNUSED(regs);

    uint8_t byte = ps2_read(PS2_DATA);

    // Try to stay synchronized by discarding obviously out of place bytes
    if (current_byte == 0 && !(byte & MOUSE_ALWAYS_SET)) {
        return;
    }

    packet[current_byte] = byte;
    current_byte = (current_byte + 1) % bytes_per_packet;

    // We've received a full packet
    if (current_byte == 0) {
        mouse_handle_packet();
    }
}

void mouse_set_callback(mouse_callback_t cb) {
    callback = cb;
}

bool mouse_states_equal(mouse_t* a, mouse_t* b) {
    return a->x == b->x && a->y == b->y &&
           a->left_pressed == b->left_pressed &&
           a->middle_pressed == b->middle_pressed &&
           a->right_pressed == b->right_pressed;
}

void mouse_handle_packet() {
    mouse_t old_state = state;

    uint8_t flags = packet[0];
    int32_t delta_x = (int32_t) packet[1];
    int32_t delta_y = (int32_t) packet[2];
    uint8_t extra = 0;

    // Superior mice send four bytes
    if (bytes_per_packet == 4) {
        extra = packet[3];

        if (extra & MOUSE_UNUSED_A || extra & MOUSE_UNUSED_B) {
            return; // Unused bits are set: beware
        }
    }

    // Packets with X or Y overflow are probably garbage
    if (flags & MOUSE_X_OVERFLOW || flags & MOUSE_Y_OVERFLOW) {
        return;
    }

    // Two's complement by hand
    if (flags & MOUSE_X_NEG) {
        delta_x |= 0xFFFFFF00;
    }

    if (flags & MOUSE_Y_NEG) {
        delta_y |= 0xFFFFFF00;
    }

    state.left_pressed = flags & MOUSE_LEFT;
    state.right_pressed = flags & MOUSE_RIGHT;
    state.middle_pressed = flags & MOUSE_MIDDLE;

    state.x += delta_x;
    state.y -= delta_y; // Point the y-axis downward

    if (!mouse_states_equal(&old_state, &state) && callback) {
        callback(state);
    }
}

void mouse_set_sample_rate(uint8_t rate) {
    ps2_write_device(device, MOUSE_SET_SAMPLE);
    ps2_expect_ack();
    ps2_write_device(device, rate);
    ps2_expect_ack();
}

void mouse_set_resolution(uint8_t level) {
    ps2_write_device(device, MOUSE_SET_RESOLUTION);
    ps2_expect_ack();
    ps2_write_device(device, level);
    ps2_expect_ack();
}

void mouse_set_scaling(bool enabled) {
    uint8_t cmd = enabled ? MOUSE_ENABLE_SCALING : MOUSE_DISABLE_SCALING;

    ps2_write_device(device, cmd);
    ps2_expect_ack();
}

/* Uses a magic sequence to enable scroll wheel support.
 */
void mouse_enable_scroll_wheel() {
    mouse_set_sample_rate(200);
    mouse_set_sample_rate(100);
    mouse_set_sample_rate(80);

    uint32_t type = ps2_identify_device(device);

    if (type == PS2_MOUSE_SCROLL_WHEEL) {
        bytes_per_packet = 4;
        debug_writeln("enabled scroll wheel");
    } else {
        debug_writeln("unable to enable scroll wheel");
    }
}

/* Uses a magic sequence to enable five buttons support.
 */
void mouse_enable_five_buttons() {
    if (bytes_per_packet != 4) {
        return;
    }

    mouse_set_sample_rate(200);
    mouse_set_sample_rate(200);
    mouse_set_sample_rate(80);

    uint32_t type = ps2_identify_device(device);

    if (type != PS2_MOUSE_FIVE_BUTTONS) {
        debug_writeln("mouse has fewer than five buttons");
    } else {
        debug_writeln("five buttons enabled");
    }
}