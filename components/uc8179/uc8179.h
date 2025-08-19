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

// POWER OFF SEQUENCE SETTING (PFS) (R03H)
typedef enum { // Source to gate power off interval time (in frames)
    PFS_T_VDS_OFF_1 = 0b00000000,
    PFS_T_VDS_OFF_2 = 0b00110000,
    PFS_T_VDS_OFF_3 = 0b00110000,
    PFS_T_VDS_OFF_4 = 0b00110000,
} PFS_T_VDS_OFF;

// BOOSTER SOFT START (BTST) (R06H)
typedef enum { // Soft start period of phase
    BTST_BT_PHASE_PERIOD_10MS = 0b00000000, // default
    BTST_BT_PHASE_PERIOD_20MS = 0b01000000,
    BTST_BT_PHASE_PERIOD_30MS = 0b10000000,
    BTST_BT_PHASE_PERIOD_40MS = 0b11000000,
} BTST_BT_PHASE_PERIOD;

typedef enum { // Driving strength of phase
    BTST_BT_PHASE_STRENGTH_1 = 0b00000000,
    BTST_BT_PHASE_STRENGTH_2 = 0b00001000,
    BTST_BT_PHASE_STRENGTH_3 = 0b00010000,  // default
    BTST_BT_PHASE_STRENGTH_4 = 0b00011000,
    BTST_BT_PHASE_STRENGTH_5 = 0b00100000,
    BTST_BT_PHASE_STRENGTH_6 = 0b00101000,
    BTST_BT_PHASE_STRENGTH_7 = 0b00110000,
    BTST_BT_PHASE_STRENGTH_8 = 0b00111000,  // strongest
} BTST_BT_PHASE_STRENGTH;

typedef enum { // Minimum OFF time setting of GDR in phase
    BTST_BT_PHASE_OFF_0_27_US = 0b00000000,
    BTST_BT_PHASE_OFF_0_34_US = 0b00000001,
    BTST_BT_PHASE_OFF_0_40_US = 0b00000010,
    BTST_BT_PHASE_OFF_0_54_US = 0b00000011,
    BTST_BT_PHASE_OFF_0_80_US = 0b00000100,
    BTST_BT_PHASE_OFF_1_54_US = 0b00000101,
    BTST_BT_PHASE_OFF_3_34_US = 0b00000110,
    BTST_BT_PHASE_OFF_6_58_US = 0b00000111, // default
} BTST_BT_PHASE_OFF;

typedef struct {
    BTST_BT_PHASE_PERIOD period;
    BTST_BT_PHASE_STRENGTH strength;
    BTST_BT_PHASE_OFF off_time;
} BTST_BT_PHASE;
#define BTST_BT_PHASE_CREATE(PHASE) ((uint8_t)PHASE.period | (uint8_t)PHASE.strength | (uint8_t)PHASE.off_time)

typedef enum { // Booster phase-C2 enable
    BTST_PHC2EN_DISABLE = 0x00, // Booster phase-C2 disable
                                // Phase-C1 setting always is applied for booster phase-C.
    BTST_PHC2EN_ENABLE = 0x80,  // Booster phase-C2 enable
                                // If temperature > temperature boundary phase-C2(RE7h[7:0]), phase-C1 setting is applied for booster phase-C.
                                // If temperature <= temperature boundary phase-C2(RE7h[7:0]), phase-C2 setting is applied for booster phase-C.
} BTST_PHC2EN;

// DATA STOP (DSP) (R11H)
typedef enum {
    DSP_DATA_FLAG_NOT_RECEIVED = 0x00,
    DSP_DATA_FLAG_RECEIVED = 0x80,
} DSP_DATA_FLAG;

// DUAL SPI MODE (DUSPI) (R15H)
typedef enum { // MM input pin definition enable.
    DUSPI_MM_EN_DISABLE = 0x00,
    DUSPI_MM_EN_ENABLE = 0x20,
} DUSPI_MM_EN;

typedef enum { // Dual SPI mode enable
    DUSPI_DUSPI_EN_DISABLE = 0x00,
    DUSPI_DUSPI_EN_ENABLE = 0x10,
} DUSPI_DUSPI_EN;

// VCOM AND DATA INTERVAL SETTING (CDI) (R50H)
typedef enum { // Border Hi-Z control
    CDI_BDZ_DISABLED = 0x00,    // default
    CDI_BDZ_ENABLED = 0x80,
} CDI_BDZ;

typedef enum { // Border LUT selection
    // Values repeat based on scenario
    // KW without R mode
        // DDX[0] = 0 <=> W=0
            CDI_BDV_KW_W0K1_LUTBD = 0x00,
            CDI_BDV_KW_W0K1_LUTKW = 0x01,
            CDI_BDV_KW_W0K1_LUTWK = 0x02,
            CDI_BDV_KW_W0K1_LUTKK = 0x03,
        // DDX[0] = 1 <=> W=1
            CDI_BDV_KW_W1K0_LUTKK = 0x00,
            CDI_BDV_KW_W1K0_LUTWK = 0x01,
            CDI_BDV_KW_W1K0_LUTKW = 0x02,
            CDI_BDV_KW_W1K0_LUTBD = 0x03,
    // KWR mode
        // DDX[0] = 0
            CDI_BDV_KWR_W0K1_LUTBD = 0x00,
            CDI_BDV_KWR_W0K1_LUTR = 0x01,
            CDI_BDV_KWR_W0K1_LUTW = 0x02,
            CDI_BDV_KWR_W0K1_LUTK = 0x03,
        // DDX[0] = 1
            CDI_BDV_KWR_W1K0_LUTK = 0x00,
            CDI_BDV_KWR_W1K0_LUTW = 0x01,
            CDI_BDV_KWR_W1K0_LUTR = 0x02,
            CDI_BDV_KWR_W1K0_LUTBD = 0x03,
} CDI_BDV;

typedef enum { // Copy frame data from NEW data to OLD data enable control after display refresh with NEW/OLD in KW mode
    CDI_N2OCP_DISABLED = 0x00,  // default
    CDI_N2OCP_ENABLED = 0x08,
} CDI_N2OCP;

typedef enum { // Data polarity
    // KW without R mode
        // DDX[1]=0 is for KW mode with NEW/OLD
            CDI_DDX_KW_N_O_W0K1 = 0x00,
                //  Data{new, old}  LUT
                //  00              LUTWW (0 -> 0)
                //  01              LUTKW (1 -> 0)
                //  10              LUTWK (0 -> 1)
                //  11              LUTKK (1 -> 1)
            CDI_DDX_KW_N_O_W1K0 = 0x01,   // default
                //  Data{new, old}  LUT
                //  00              LUTKK (0 -> 0)
                //  01              LUTWK (1 -> 0)
                //  10              LUTKW (0 -> 1)
                //  11              LUTWW (1 -> 1)
        // DDX[1]=1 is for KW mode without NEW/OLD.
            CDI_DDX_KW_N_W0K1 = 0x02,
                //  Data{new}   LUT
                //  0           LUTKW (1 -> 0)
                //  1           LUTWK (0 -> 1)
            CDI_DDX_KW_N_W1K0 = 0x03,
                //  Data{new}   LUT
                //  0           LUTWK (1 -> 0)
                //  1           LUTKW (0 -> 1)
    // KWR mode
        CDI_DDX_KWR_W0K1R1 = 0x00,
                //  Data{red, KW}   LUT
                //  00              LUTW
                //  01              LUTK
                //  10              LUTR
                //  11              LUTR
        CDI_DDX_KWR_W1K0R1 = 0x01,    // default
                //  Data{red, KW}   LUT
                //  00              LUTK
                //  01              LUTW
                //  10              LUTR
                //  11              LUTR
        CDI_DDX_KWR_W0K1R0 = 0x02,
                //  Data{red, KW}   LUT
                //  00              LUTR
                //  01              LUTR
                //  10              LUTW
                //  11              LUTK
        CDI_DDX_KWR_W1K0R0 = 0x03,
                //  Data{red, KW}   LUT
                //  00              LUTR
                //  01              LUTR
                //  10              LUTK
                //  11              LUTW
} CDI_DDX;

typedef enum { // number of hsyncs between VCOM ready and source data output
    CDI_CDI_17_HSYNC = 0x00,
    CDI_CDI_16_HSYNC = 0x01,
    CDI_CDI_15_HSYNC = 0x02,
    CDI_CDI_14_HSYNC = 0x03,
    CDI_CDI_13_HSYNC = 0x04,
    CDI_CDI_12_HSYNC = 0x05,
    CDI_CDI_11_HSYNC = 0x06,
    CDI_CDI_10_HSYNC = 0x07, // default
    CDI_CDI_9_HSYNC = 0x08,
    CDI_CDI_8_HSYNC = 0x09,
    CDI_CDI_7_HSYNC = 0x0A,
    CDI_CDI_6_HSYNC = 0x0B,
    CDI_CDI_5_HSYNC = 0x0C,
    CDI_CDI_4_HSYNC = 0x0D,
    CDI_CDI_3_HSYNC = 0x0E,
    CDI_CDI_2_HSYNC = 0x0F,
} CDI_CDI;

// END VOLTAGE SETTING (EVS) (R52H)
typedef enum { // VCOM end voltage selection
    EVS_VCEND_VCOM_DC = 0x00, // default
    EVS_VCEND_FLOATING = 0x08,
} EVS_VCEND;

typedef enum { // Border end voltage selection
    EVS_BDEND_0 = 0x00,
    EVS_BDEND_VCOM_DC = 0x02,   // default
    EVS_BDEND_FLOATING = 0x03,
} EVS_BDEND;
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

    void cmd_power_off() {
        // After the Power OFF command, the driver will be powered OFF. Refer to the POWER MANAGEMENT section for the sequence.
        // This command will turn off booster, controller, source driver, gate driver, VCOM, and temperature sensor, but register data will be
        // kept until VDD turned OFF or Deep Sleep Mode. Source/Gate/Border/VCOM will be released to floating.
        this->command(0x02);
        this->wait_until_idle_();
    }

    void cmd_power_off_sequence_setting(PFS_T_VDS_OFF source_to_gate_min_off_interval) {
        this->command(0x03);
        this->data(source_to_gate_min_off_interval);
    }

    void cmd_power_on() {
        // After the Power ON command, the driver will be powered ON. Refer to the POWER MANAGEMENT section for the sequence.
        // This command will turn on booster, controller, regulators, and temperature sensor will be activated for one-time sensing before
        // enabling booster. When all voltages are ready, the BUSY_N signal will return to high.
        this->command(0x04);
        this->wait_until_idle_();
    }

    void cmd_power_on_measure() {
        // This command enables the internal bandgap, which will be cleared by the next POF.
        this->command(0x05);
        this->wait_until_idle_();
    }

    void cmd_booster_soft_start(BTST_BT_PHASE a, BTST_BT_PHASE b, BTST_BT_PHASE c1, BTST_PHC2EN c2_enabled, BTST_BT_PHASE c2) {
        this->command(0x06);
        this->data(BTST_BT_PHASE_CREATE(a));
        this->data(BTST_BT_PHASE_CREATE(b));
        this->data(BTST_BT_PHASE_CREATE(c1));
        this->data(BTST_BT_PHASE_CREATE(c2) & (~0x80) | (uint8_t)c2_enabled);
    }

    void cmd_deep_sleep() {
        // After this command is transmitted, the chip will enter Deep Sleep Mode to save power. Deep Sleep Mode will return to Standby
        // Mode by hardware reset. The only one parameter is a check code, the command will be executed if check code = 0xA5.
        this->command(0x07);
        this->data(0xA5);
        this->wait_until_idle_();
    }

    void cmd_data_start_transmission_1(const uint8_t *data, size_t length) {
        // This command starts transmitting data and write them into SRAM.
        // In KW mode, this command writes “OLD” data to SRAM.
        // In KWR mode, this command writes “K/W” data to SRAM.
        // In Program mode, this command writes “OTP” data to SRAM for programming.
        this->command(0x10);
        this->data(data, length);
        this->wait_until_idle_();
    }

    void cmd_data_stop(DSP_DATA_FLAG data_received) {
        // Check the completeness of data. If data is complete, start to refresh display
        this->command(0x11);
        this->data(data_received);
        if(data_received == DSP_DATA_FLAG_RECEIVED) {
            this->wait_until_idle_();
        }
    }

    void cmd_display_refresh() {
        // While user sent this command, driver will refresh display (data/VCOM) according to SRAM data and LUT.
        // After Display Refresh command, BUSY_N signal will become “0” and the refreshing of panel starts.
        this->command(0x12);
        this->wait_until_idle_();
    }

    void cmd_data_start_transmission_2(const uint8_t *data, size_t length) {
        // This command starts transmitting data and write them into SRAM.
        // In KW mode, this command writes “NEW” data to SRAM.
        // In KWR mode, this command writes “RED” data to SRAM.
        this->command(0x13);
        this->data(data, length);
        this->wait_until_idle_();
    }

    void cmd_dual_spi_mode(DUSPI_MM_EN mm_pin, DUSPI_DUSPI_EN dual_spi) {
        this->command(0x15);
        this->data((uint8_t)mm_pin | (uint8_t)dual_spi);
    }

    void cmd_vcom_data_interval_setting(CDI_BDZ border_hi_impedance, CDI_BDV lut_selection, CDI_N2OCP copy_new_to_old, CDI_DDX data_polarity, CDI_CDI vcom_data_interval) {
        // This command indicates the interval of VCOM and data output. When setting the vertical back porch, the total blanking will be kept (20 Hsync)
        this->command(0x50);
        this->data((uint8_t)border_hi_impedance | (uint8_t)lut_selection | (uint8_t)copy_new_to_old | (uint8_t)data_polarity);
        this->data(vcom_data_interval);
    }

    void cmd_end_voltage_setting(EVS_VCEND vcom_end_voltage, EVS_BDEND border_end_voltage) {
        // This command selects source end voltage and border end voltage after LUTs are finished.
        this->command(0x52);
        this->data((uint8_t)vcom_end_voltage | (uint8_t)border_end_voltage);
    }

    void cmd_resolution_setting(uint hres, uint vres) {
        this->command(0x61);
        this->data(hres>>8);
        this->data(hres&0xF8);
        this->data(vres>>8);
        this->data(vres&0xFF);
    }

protected:
    bool wait_until_idle_();
    uint32_t idle_timeout_{1000u};
};

} // namespace uc8179
} // namespace esphome