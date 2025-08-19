#include "esphome/core/log.h"
#include "display.h"

namespace esphome
{
namespace uc8179
{

static const char *TAG = "uc8179.display";

void UC8179DisplayBase::update() {
    this->do_update_();
    this->display();
}

void UC8179DisplayBase::setup() {
    this->driver_->reset();

    this->driver_->cmd_power_setting(PWR_BD_EN_OFF, PWR_VSR_EN_INTERNAL, PWR_VS_EN_INTERNAL, PWR_VG_EN_INTERNAL, PWR_VCOM_SLEW_DEFAULT, PWR_VG_LVL_20, PWR_VDH_LVL_15, PWR_VDL_LVL_15, PWR_VDHR_LVL_3);

    BTST_BT_PHASE std_phase;
    std_phase.period = BTST_BT_PHASE_PERIOD_10MS;
    std_phase.strength = BTST_BT_PHASE_STRENGTH_3;
    std_phase.off_time = BTST_BT_PHASE_OFF_6_58_US;
    BTST_BT_PHASE c1_phase;
    c1_phase.period = BTST_BT_PHASE_PERIOD_10MS;
    c1_phase.strength = BTST_BT_PHASE_STRENGTH_2;
    c1_phase.off_time = BTST_BT_PHASE_OFF_0_27_US;
    this->driver_->cmd_booster_soft_start(std_phase, std_phase, c1_phase, BTST_PHC2EN_DISABLE, std_phase);

    this->init_internal_(this->get_buffer_size_());
}

void UC8179DisplayBase::setup_panel() {
    this->driver_->cmd_panel_setting(this->lut_location_, this->kwr_mode_, this->gate_scan_dir_, this->source_shift_dir_, this->booster_switch_, PSR_RST_N_NO_EFFECT);
}

void UC8179DisplayBase::display() {
    this->initialize();
    this->send_buffer_internal_();
    this->driver_->cmd_display_refresh();
}

void UC8179DisplayBase::deep_sleep() {
    this->driver_->cmd_power_off();
    this->driver_->cmd_deep_sleep();
}

void UC8179DisplayBase::initialize() {
    this->driver_->reset();
    this->driver_->cmd_power_on();
}

void UC8179Display_KW::initialize() {
    UC8179DisplayBase::initialize();
    this->kwr_mode_ = PSR_KWR_KW;
    this->setup_panel();
}

void UC8179Display_KW::send_buffer_internal_() {
    // KW mode, so we need to send OLD data and NEW data
    this->driver_->cmd_data_start_transmission_1(this->buffer_, this->get_buffer_size_());
    this->driver_->cmd_data_start_transmission_2(this->buffer_, this->get_buffer_size_());
}

void UC8179Display_KW::draw_absolute_pixel_internal(int x, int y, Color color) {
    if (x >= this->get_width_internal() || y >= this->get_height_internal() || x < 0 || y < 0)
        return;

    const uint32_t pos = (x + y * this->get_width_internal()) / 8u;
    const uint8_t subpos = x & 0x07;
    if (!color.is_on()) {
        this->buffer_[pos] |= 0x80 >> subpos;
    } else {
        this->buffer_[pos] &= ~(0x80 >> subpos);
    }
}

void UC8179Display_G4::draw_absolute_pixel_internal(int x, int y, Color color) {
    if (x >= this->get_width_internal() || y >= this->get_height_internal() || x < 0 || y < 0)
    return;

    const uint8_t brightness = std::max({color.red, color.green, color.blue, color.white});
    uint8_t color_bitmap = 0x00;
    if (brightness > 0x3F)        // >25%
        if (brightness > 0x7F)      // >50%
            if (brightness > 0xBF)    // >75%
                color_bitmap = 0xC0;
            else
                color_bitmap = 0x80;
        else
            color_bitmap = 0x40;

    const uint32_t pos = (x + y * this->get_width_internal()) / 4u;
    const uint8_t subpos = (x & 0x03) << 1;  // number of bits to shift, = x%4 * 2

    this->buffer_[pos] &= ~(0xC0 >> subpos);
    this->buffer_[pos] |= (color_bitmap >> subpos);
}

} // namespace uc8179
} // namespace esphome