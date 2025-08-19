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

// POWER SETTING (PWR) (R01H)
typedef enum { // Border LDO enable
    PWR_BD_EN_OFF = 0x00,   // Border LDO disable (Default)
                            // Border level selection: 00b: VCOM 01b: VDH 10b: VDL 11b: VDHR
    PWR_BD_EN_ON = 0x10,    //  Border LDO enable
                            // Border level selection: 00b: VCOM 01b: VBH(VCOM-VDL) 10b:VBL(VCOM-VDH) 11b: VDHR
} PWR_BD_EN;

typedef enum { // Source LV power selection
    PWR_VSR_EN_EXTERNAL = 0x00, // External source power from VDHR pins
    PWR_VSR_EN_INTERNAL = 0x04, // Internal DC/DC function for generating VDHR. (Default)
} PWR_VSR_EN;

typedef enum { // Source power selection
    PWR_VS_EN_EXTERNAL = 0x00,  // External source power from VDH/VDL pins
    PWR_VS_EN_INTERNAL = 0x02,  // Internal DC/DC function for generating VDH/VDL. (Default)
} PWR_VS_EN;

typedef enum { // Gate power selection
    PWR_VG_EN_EXTERNAL = 0x00,  // External gate power from VGH/VGL pins
    PWR_VG_EN_INTERNAL = 0x01,  // Internal DC/DC function for generating VGH/VGL. (Default)
} PWR_VG_EN;

typedef enum {
    PWR_VCOM_SLEW_DEFAULT = 0x10,    // VCOM slew rate selection for voltage transition. The value is fixed at “1”.
} PWR_VCOM_SLEW;

typedef enum { // VGH / VGL Voltage Level selection.
    PWR_VG_LVL_9  = 0x00,   // VGH= 9V, VGL= -9V
    PWR_VG_LVL_10 = 0x01,   // VGH= 10V, VGL= -10V
    PWR_VG_LVL_11 = 0x02,   // VGH= 11V, VGL= -11V
    PWR_VG_LVL_12 = 0x03,   // VGH= 12V, VGL= -12V
    PWR_VG_LVL_17 = 0x04,   // VGH= 17V, VGL= -17V
    PWR_VG_LVL_18 = 0x05,   // VGH= 18V, VGL= -18V
    PWR_VG_LVL_19 = 0x06,   // VGH= 19V, VGL= -19V
    PWR_VG_LVL_20 = 0x07,   // VGH= 20V, VGL= -20V
} PWR_VG_LVL;

typedef enum { // Internal VDH power selection for K/W pixel.(Default value: 111010b)
    PWR_VDH_LVL_2_4 = 0b00000000,   // min
    PWR_VDH_LVL_3_6 = 0b00000110,
    PWR_VDH_LVL_5 = 0b00001101,
    PWR_VDH_LVL_9 = 0b00100010,
    PWR_VDH_LVL_12 = 0b00110000,
    PWR_VDH_LVL_14 = 0b00111010,    // default
    PWR_VDH_LVL_15 = 0b00111111,    // max
} PWR_VDH_LVL;

typedef enum { // Internal VDL power selection for K/W pixel. (Default value: 111010b)
    PWR_VDL_LVL_2_4 = 0b00000000,   // max (negative)
    PWR_VDL_LVL_3_6 = 0b00000110,
    PWR_VDL_LVL_5 = 0b00001101,
    PWR_VDL_LVL_9 = 0b00100010,
    PWR_VDL_LVL_12 = 0b00110000,
    PWR_VDL_LVL_14 = 0b00111010,    // default
    PWR_VDL_LVL_15 = 0b00111111,    // min (negative)
} PWR_VDL_LVL;

typedef enum { // Internal VDHR power selection for Red pixel. (Default value: 000011b)
    PWR_VDHR_LVL_2_4 = 0b00000000,   // min
    PWR_VDHR_LVL_3 = 0b00000011,     // default
    PWR_VDHR_LVL_3_6 = 0b00000110,
    PWR_VDHR_LVL_5 = 0b00001101,
    PWR_VDHR_LVL_9 = 0b00100010,
    PWR_VDHR_LVL_12 = 0b00110000,
    PWR_VDHR_LVL_15 = 0b00111111,    // max
} PWR_VDHR_LVL;
class UC8179 : public UC8179Base
{
public:
    void set_idle_timeout(uint32_t idle_timeout) { this->idle_timeout_ = idle_timeout; }

    void cmd_panel_setting(PSR_REG lut_location, PSR_KWR kwr, PSR_UD gate_scan_dir, PSR_SHL source_shift_dir, PSR_SHD_N booster_switch, PSR_RST_N reset) {
        this->command(0x00);
        this->data((uint8_t)lut_location | (uint8_t)kwr | (uint8_t)gate_scan_dir | (uint8_t)source_shift_dir | (uint8_t)booster_switch | (uint8_t)reset);
    }

    void cmd_power_setting(PWR_BD_EN border_ldo, PWR_VSR_EN source_lv_power, PWR_VS_EN source_power, PWR_VG_EN gate_power, PWR_VCOM_SLEW slew_rate, PWR_VG_LVL voltage_level, PWR_VDH_LVL vdh_power, PWR_VDL_LVL vdl_power, PWR_VDHR_LVL vdhr_power) {
        this->command(0x01);
        this->data((uint8_t)border_ldo | (uint8_t)source_lv_power | (uint8_t)source_power | (uint8_t)gate_power);
        this->data((uint8_t)slew_rate | (uint8_t)voltage_level);
        this->data(vdh_power);
        this->data(vdl_power);
        this->data(vdhr_power);
    }


protected:
    bool wait_until_idle_();
    uint32_t idle_timeout_{1000u};
};

} // namespace uc8179
} // namespace esphome