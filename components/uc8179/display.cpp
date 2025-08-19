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

    size_t size = this->get_width() * this->get_height() / this->pixels_per_byte_();
    this->init_internal_(size);
}

void UC8179DisplayBase::setup_panel() {
    this->driver_->cmd_panel_setting(this->lut_location_, this->kwr_mode_, this->gate_scan_dir_, this->source_shift_dir_, this->booster_switch_, PSR_RST_N_NO_EFFECT);
}

void UC8179DisplayBase::send_buffer_internal_() {
}

void UC8179DisplayBase::initialize() {
    this->driver_->reset();
    this->driver_->cmd_power_on();
}

} // namespace uc8179
} // namespace esphome