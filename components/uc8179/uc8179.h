#pragma once

#include "esphome/core/component.h"
#include "esphome/components/spi/spi.h"

namespace esphome
{
namespace uc8179
{

class UC8179Base : public Component, public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW, spi::CLOCK_PHASE_LEADING, spi::DATA_RATE_2MHZ>
{
public:
    void setup() override;
    void loop() override;
    void dump_config() override;

    void set_dc_pin(GPIOPin *dc_pin) { this->dc_pin_ = dc_pin; }
    void set_busy_pin(GPIOPin *busy_pin) { this->busy_pin_ = busy_pin; }
    void set_reset_pin(GPIOPin *reset_pin) { this->reset_pin_ = reset_pin; }

    void command(uint8_t value);
    void data(uint8_t value);
    void data(const uint8_t *data, size_t length);

    void reset() {
        if (this->reset_pin_ != nullptr) {
            this->reset_pin_->digital_write(false);
            delay(2); // min 50us
            this->reset_pin_->digital_write(true);
            delay(20);
        }
    }

protected:
    GPIOPin *dc_pin_{nullptr};
    GPIOPin *busy_pin_{nullptr};
    GPIOPin *reset_pin_{nullptr};

    void start_command_();
    void end_command_();
    void start_data_();
    void end_data_();
};

// PANEL SETTING (PSR) (REGISTER: R00H)
typedef enum { // LUT selection
    PSR_REG_LUT_FROM_OTP = 0x00, // LUT from OTP. (Default)
    PSR_REG_LUT_FROM_REG = 0x20, // LUT from register.
} PSR_REG;

typedef enum { // Black / White / Red
    PSR_KWR_KWR = 0x00, // Pixel with Black/White/Red, KWR mode. (Default)
    PSR_KWR_KW = 0x10,  // Pixel with Black/White, KW mode
} PSR_KWR;

typedef enum { // Gate Scan Direction
    PSR_UD_DOWN = 0x00, // Scan down. First line to Last line: Gn-1  Gn-2  Gn-3  …  G0
    PSR_UD_UP = 0x08,   // Scan up. (Default) First line to Last line: G0  G1  G2  … … .  Gn-1
} PSR_UD;

typedef enum { // Source Shift Direction
    PSR_SHL_LEFT = 0x00,    // Shift left. First data to Last data: Sn-1  Sn-2  Sn-3  …  S0
    PSR_SHL_RIGHT = 0x04,   // Shift right. (Default) First data to Last data: S0  S1  S2  … … .  Sn-1
} PSR_SHL;

typedef enum { // Booster Switch
    PSR_SHD_N_ON = 0x00, // Booster OFF
    PSR_SHD_N_OFF = 0x02, // Booster ON (Default)
} PSR_SHD_N;

typedef enum { // Soft Reset
    PSR_RST_N_RESET = 0x00,     // Reset. Booster OFF, Register data are set to their default values, all drivers will be reset, and all functions will be disabled. Source/Gate/Border/VCOM will be released to floating
    PSR_RST_N_NO_EFFECT = 0x01, // No effect (Default).
} PSR_RST_N;
class UC8179 : public UC8179Base
{
public:
    void set_idle_timeout(uint32_t idle_timeout) { this->idle_timeout_ = idle_timeout; }

    void cmd_panel_setting(PSR_REG lut_location, PSR_KWR kwr, PSR_UD gate_scan_dir, PSR_SHL source_shift_dir, PSR_SHD_N booster_switch, PSR_RST_N reset) {
        this->command(0x00);
        this->data((uint8_t)lut_location | (uint8_t)kwr | (uint8_t)gate_scan_dir | (uint8_t)source_shift_dir | (uint8_t)booster_switch | (uint8_t)reset);
    }


protected:
    bool wait_until_idle_();
    uint32_t idle_timeout_{1000u};
};

} // namespace uc8179
} // namespace esphome