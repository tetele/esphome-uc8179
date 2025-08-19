#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include "uc8179.h"

namespace esphome
{
namespace uc8179
{

class UC8179DisplayBase : public display::DisplayBuffer {
public:
    void set_driver(UC8179 *driver) { this->driver_ = driver; }
    void update() override;
    void setup() override;
    void setup_panel();

    virtual void display();

protected:
    UC8179 *driver_{nullptr};
    virtual uint32_t pixels_per_byte_();

    PSR_REG lut_location_{PSR_REG_LUT_FROM_OTP};
    PSR_KWR kwr_mode_{PSR_KWR_KW};
    PSR_UD gate_scan_dir_{PSR_UD_UP};
    PSR_SHL source_shift_dir_{PSR_SHL_RIGHT};
    PSR_SHD_N booster_switch_{PSR_SHD_N_OFF};

    void initialize();

    size_t get_buffer_size_() { return this->get_width() * this->get_height() / this->pixels_per_byte_(); }
    virtual void send_buffer_internal_() = 0;
};

// Color scales

class UC8179Display_KW : public UC8179DisplayBase {
public:
    display::DisplayType get_display_type() override { return display::DISPLAY_TYPE_BINARY; }

    void initialize();

protected:
    uint32_t pixels_per_byte_() { return 8; }

    void send_buffer_internal_() override;
    void draw_absolute_pixel_internal(int x, int y, Color color) override;
};

class UC8179Display_G4 : public UC8179DisplayBase {
public:
    display::DisplayType get_display_type() override { return display::DISPLAY_TYPE_GRAYSCALE; }

    void initialize();

protected:
    uint32_t pixels_per_byte_() { return 4; }

    void send_buffer_internal_() override;
    void draw_absolute_pixel_internal(int x, int y, Color color) override;
};

} // namespace uc8179
} // namespace esphome