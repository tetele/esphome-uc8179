#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include "uc8179.h"

namespace esphome
{
namespace uc8179
{

typedef enum {
    LUT_LOCATION_OTP,
    LUT_LOCATION_REGISTER,
} lut_location_t;

class UC8179DisplayBase : public display::DisplayBuffer {
public:
    void set_driver(UC8179 *driver) { this->driver_ = driver; }
    void update() override;
    void setup() override;
    void setup_panel(PSR_KWR kwr_mode);

    virtual void display() = 0;

protected:
    UC8179 *driver_{nullptr};
    virtual uint32_t pixels_per_byte_();

    lut_location_t lut_location_{LUT_LOCATION_OTP};

    void initialize();

    void send_buffer_internal_();
};

} // namespace uc8179
} // namespace esphome