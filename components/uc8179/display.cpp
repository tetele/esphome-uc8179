#include "esphome/core/log.h"
#include "display.h"

namespace esphome
{
namespace uc8179
{

static const char *TAG = "uc8179.display";

void UC8179DisplayBase::dump_config() {
    ESP_LOGCONFIG(TAG, "UC8179 driven Display");
}

void UC8179DisplayBase::update() {
    this->do_update_();
    this->display();
    this->deep_sleep();
}

void UC8179DisplayBase::setup() {
    this->init_internal_(this->get_buffer_size_());
    this->driver_->reset();
    this->driver_->set_lut_location(PSR_REG_LUT_FROM_OTP);
    this->driver_->set_kwr_mode(PSR_KWR_KW);
    this->driver_->set_gate_scan_dir(PSR_UD_UP);
    this->driver_->set_source_shift_dir(PSR_SHL_RIGHT);
    this->driver_->set_booster_switch(PSR_SHD_N_OFF);
    this->driver_->set_vcom_data_interval(CDI_CDI_10_HSYNC);
    this->driver_->setup_waveform();
}

void UC8179DisplayBase::setup_panel() {
    this->driver_->setup_panel();
    this->driver_->setup_resolution(this->get_width_internal(), this->get_height_internal());
}

void UC8179DisplayBase::display() {
    this->initialize();
    this->send_buffer_internal_();
    this->driver_->refresh();
}

void UC8179DisplayBase::deep_sleep() {
    this->driver_->power_off();
    this->driver_->deep_sleep();
}

void UC8179DisplayBase::initialize() {
    this->driver_->reset();
    this->driver_->power_on();
}

void UC8179Display_KW::initialize() {
    this->driver_->set_kwr_mode(PSR_KWR_KW);
    this->driver_->set_transmit_old_data(true);
    UC8179DisplayBase::initialize();
    this->setup_panel();
}

void UC8179Display_KW::send_buffer_internal_() {
    this->driver_->load_image_data(this->buffer_, this->get_buffer_size_());
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

void GDEY075T7::setup() {
    UC8179DisplayBase::setup();

    this->driver_->setup_power(false, true, PWR_VG_LVL_20, PWR_VDH_LVL_15, PWR_VDL_LVL_15, PWR_VDHR_LVL_3);

    BTST_BT_PHASE std_phase;
    std_phase.period = BTST_BT_PHASE_PERIOD_10MS;
    std_phase.strength = BTST_BT_PHASE_STRENGTH_3;
    std_phase.off_time = BTST_BT_PHASE_OFF_6_58_US;
    BTST_BT_PHASE c1_phase;
    c1_phase.period = BTST_BT_PHASE_PERIOD_10MS;
    c1_phase.strength = BTST_BT_PHASE_STRENGTH_2;
    c1_phase.off_time = BTST_BT_PHASE_OFF_0_27_US;
    this->driver_->setup_booster_soft_start(std_phase, std_phase, c1_phase, BTST_PHC2EN_DISABLE, std_phase);

    this->driver_->setup_dual_spi(false);

    this->driver_->set_non_overlap_period(TCON_G2S_12);
}

} // namespace uc8179
} // namespace esphome