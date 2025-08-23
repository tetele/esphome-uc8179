#include "esphome/core/log.h"
#include "model_gdey075t7.h"

namespace esphome
{
namespace uc8179
{

static const char *TAG = "uc8179.display.gdey075t7";

void GDEY075T7::dump_config() {
    ESP_LOGCONFIG(TAG, "GDEY075T7 display");
    ESP_LOGCONFIG(TAG, "  Resolution: %dx%d px", this->get_width(), this->get_height());
}

void GDEY075T7::setup() {
    UC8179Display_G4::setup();

    this->driver_->setup_power(false, true, PWR_VG_LVL_20, PWR_VDH_LVL_15, PWR_VDL_LVL_15, PWR_VDHR_LVL_3);

    this->driver_->setup_dual_spi(false);

    this->driver_->set_non_overlap_period(TCON_G2S_12);

    this->post_setup();
}

void GDEY075T7::initialize() {
    UC8179Display_G4::initialize();

    BTST_BT_PHASE std_phase;
    std_phase.period = BTST_BT_PHASE_PERIOD_10MS;
    std_phase.strength = BTST_BT_PHASE_STRENGTH_5;
    std_phase.off_time = BTST_BT_PHASE_OFF_6_58_US;
    BTST_BT_PHASE c1_phase;
    c1_phase.period = BTST_BT_PHASE_PERIOD_10MS;
    c1_phase.strength = BTST_BT_PHASE_STRENGTH_4;
    c1_phase.off_time = BTST_BT_PHASE_OFF_0_27_US;
    BTST_BT_PHASE c2_phase;
    c2_phase.period = BTST_BT_PHASE_PERIOD_10MS;
    c2_phase.strength = BTST_BT_PHASE_STRENGTH_3;
    c2_phase.off_time = BTST_BT_PHASE_OFF_6_58_US;
    this->driver_->setup_booster_soft_start(std_phase, std_phase, c1_phase, BTST_PHC2EN_DISABLE, c2_phase);

    this->driver_->setup_fixed_temperature(0x5F); // according to demo
}

void GDEY075T7_BW::dump_config() {
    ESP_LOGCONFIG(TAG, "GDEY075T7 (Black/White) display");
    ESP_LOGCONFIG(TAG, "  Resolution: %dx%d px", this->get_width(), this->get_height());
}

void GDEY075T7_BW::setup() {
    UC8179Display_KW::setup();
    this->driver_->set_copy_new_to_old(CDI_N2OCP_DISABLED);

    this->driver_->setup_power(false, true, PWR_VG_LVL_20, PWR_VDH_LVL_15, PWR_VDL_LVL_15, PWR_VDHR_LVL_3);

    BTST_BT_PHASE std_phase;
    std_phase.period = BTST_BT_PHASE_PERIOD_10MS;
    std_phase.strength = BTST_BT_PHASE_STRENGTH_3;
    std_phase.off_time = BTST_BT_PHASE_OFF_6_58_US;
    BTST_BT_PHASE c1_phase;
    c1_phase.period = BTST_BT_PHASE_PERIOD_10MS;
    c1_phase.strength = BTST_BT_PHASE_STRENGTH_6;
    c1_phase.off_time = BTST_BT_PHASE_OFF_0_27_US;
    this->driver_->setup_booster_soft_start(std_phase, std_phase, c1_phase, BTST_PHC2EN_DISABLE, std_phase);

    this->driver_->setup_dual_spi(false);

    this->driver_->set_non_overlap_period(TCON_G2S_12);

    this->post_setup();
}

} // namespace uc8179
} // namespace esphome