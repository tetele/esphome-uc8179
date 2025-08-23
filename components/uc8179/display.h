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
    virtual void deep_sleep();

protected:
    UC8179 *driver_{nullptr};
    virtual uint32_t pixels_per_byte_();

    virtual void initialize();

    size_t get_buffer_size_() { return this->get_width() * this->get_height() / this->pixels_per_byte_(); }
    virtual void send_buffer_internal_() = 0;
};

// Color scales

class UC8179Display_KW : public UC8179DisplayBase {
public:
    display::DisplayType get_display_type() override { return display::DISPLAY_TYPE_BINARY; }

    void initialize() override;

protected:
    uint32_t pixels_per_byte_() { return 8; }

    void send_buffer_internal_() override;
    void draw_absolute_pixel_internal(int x, int y, Color color) override;
};

class UC8179Display_G4 : public UC8179Display_KW {
public:
    display::DisplayType get_display_type() override { return display::DISPLAY_TYPE_GRAYSCALE; }

protected:
    uint32_t pixels_per_byte_() { return 4; }

    void draw_absolute_pixel_internal(int x, int y, Color color) override;
};

// Actual display models

class GDEY075T7_BW : public UC8179Display_KW {
public:
    void dump_config() override;
    int get_width_internal() override { return 800; }
    int get_height_internal() override { return 480; }
    void setup() override;
};

} // namespace uc8179
} // namespace esphome