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
    this->init_internal_(this->get_buffer_size_());
}

void UC8179DisplayBase::setup_panel() {
    this->driver_->cmd_panel_setting(this->lut_location_, this->kwr_mode_, this->gate_scan_dir_, this->source_shift_dir_, this->booster_switch_, PSR_RST_N_NO_EFFECT);
    this->driver_->cmd_resolution_setting(this->get_width_internal(), this->get_height_internal());

    CDI_N2OCP copy_new_to_old = CDI_N2OCP_DISABLED;
    CDI_BDV lut_selection;
    CDI_DDX data_polarity;
    if(this->kwr_mode_ == PSR_KWR_KW) {
        if(this->transmit_old_data_) {
            copy_new_to_old = CDI_N2OCP_ENABLED;
            data_polarity = CDI_DDX_KW_N_O_W0K1;
        } else {
            data_polarity = CDI_DDX_KW_N_W0K1;
        }
        lut_selection = CDI_BDV_KW_W0K1_LUTKW;
    } else {
        data_polarity = CDI_DDX_KWR_W0K1R1;
        lut_selection = CDI_BDV_KWR_W0K1_LUTBD;
    }
    this->driver_->cmd_vcom_data_interval_setting(CDI_BDZ_DISABLED, lut_selection, copy_new_to_old, data_polarity, this->vcom_data_interval_);
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
    this->transmit_old_data_ = true;
    this->setup_panel();
}

void UC8179Display_KW::send_buffer_internal_() {
    // KW mode, so we need to send OLD data and NEW data
    if(this->transmit_old_data_)
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

void GDEY075T7::setup() {
    UC8179DisplayBase::setup();

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

    this->driver_->cmd_dual_spi_mode(DUSPI_MM_EN_DISABLE, DUSPI_DUSPI_EN_DISABLE);

    this->driver_->cmd_tcon_setting(TCON_S2G_12, TCON_G2S_12);
}

} // namespace uc8179
} // namespace esphome