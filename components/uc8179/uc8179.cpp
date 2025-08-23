#include "esphome/core/log.h"
#include "uc8179.h"

namespace esphome
{
namespace uc8179
{

static const char *TAG = "uc8179.component";

void UC8179Base::setup()
{
    // Code here should perform all component initialization,
    //  whether hardware, memory, or otherwise
    this->dc_pin_->setup();
    this->dc_pin_->digital_write(false);
    if(this->reset_pin_ != nullptr) {
        this->reset_pin_->setup();
        this->reset_pin_->digital_write(true);
    }
    if(this->busy_pin_ != nullptr) {
        this->busy_pin_->setup();
    }

    this->spi_setup();
}

void UC8179Base::loop()
{
    // Tasks here will be performed at every call of the main application loop.
    // Note: code here MUST NOT BLOCK (see below)
}

void UC8179Base::dump_config()
{
    ESP_LOGCONFIG(TAG, "UC8179 EPD Driver");
    LOG_PIN("  DC Pin: ", this->dc_pin_);
    LOG_PIN("  Busy Pin: ", this->reset_pin_);
    LOG_PIN("  Reset Pin: ", this->reset_pin_);
}

void UC8179Base::command(uint8_t value) {
    this->start_command_();
    this->write_byte(value);
    this->end_command_();
}
void UC8179Base::data(uint8_t value) {
    this->start_data_();
    this->write_byte(value);
    this->end_data_();
}
void UC8179Base::data(const uint8_t *data, size_t length) {
    this->start_data_();
    this->write_array(data, length);
    this->end_data_();
}

void UC8179Base::start_command_() {
    this->dc_pin_->digital_write(false);
    this->enable();
}
void UC8179Base::end_command_() { this->disable(); }
void UC8179Base::start_data_() {
    this->dc_pin_->digital_write(true);
    this->enable();
}
void UC8179Base::end_data_() { this->disable(); }


void UC8179::setup_panel() {
    this->cmd_panel_setting(this->lut_location_, this->kwr_mode_, this->gate_scan_dir_, this->source_shift_dir_, this->booster_switch_, PSR_RST_N_NO_EFFECT);
}

void UC8179::setup_power(bool border_ldo, bool internal_power, PWR_VG_LVL voltage_level, PWR_VDH_LVL kw_high_level, PWR_VDL_LVL kw_low_level, PWR_VDHR_LVL red_level) {
    this->cmd_power_setting(
        border_ldo ? PWR_BD_EN_ON : PWR_BD_EN_OFF,
        internal_power ? PWR_VSR_EN_INTERNAL : PWR_VSR_EN_EXTERNAL,
        internal_power ? PWR_VS_EN_INTERNAL : PWR_VS_EN_EXTERNAL,
        internal_power ? PWR_VG_EN_INTERNAL : PWR_VG_EN_EXTERNAL,
        PWR_VCOM_SLEW_DEFAULT,
        voltage_level,
        kw_high_level,
        kw_low_level,
        red_level
    );
}

void UC8179::setup_dual_spi(bool dual) {
    this->cmd_dual_spi_mode(DUSPI_MM_EN_DISABLE, dual ? DUSPI_DUSPI_EN_ENABLE : DUSPI_DUSPI_EN_DISABLE);
}

void UC8179::setup_resolution(uint width, uint height) {
    this->cmd_resolution_setting(width, height);
}

void UC8179::setup_waveform() {
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
    this->cmd_vcom_data_interval_setting(CDI_BDZ_DISABLED, lut_selection, copy_new_to_old, data_polarity, this->vcom_data_interval_);

    this->cmd_tcon_setting((TCON_S2G)((this->non_overlap_period_ << 4) & 0xFF), this->non_overlap_period_);
}

void UC8179::setup_booster_soft_start(BTST_BT_PHASE a, BTST_BT_PHASE b, BTST_BT_PHASE c1, BTST_PHC2EN c2_enabled, BTST_BT_PHASE c2) {
    this->cmd_booster_soft_start(a, b, c1, c2_enabled, c2);
}

void UC8179::load_image_data(const uint8_t *data, size_t length) {
    this->cmd_data_start_transmission_2(data, length);
}

bool UC8179::wait_until_idle_() {
    if (this->busy_pin_ == nullptr || !this->busy_pin_->digital_read()) {
        return true;
    }

    const uint32_t start = millis();
    while (this->busy_pin_->digital_read()) {
        if (millis() - start > this->idle_timeout_) {
            ESP_LOGE(TAG, "Timeout while displaying image!");
            return false;
        }
        App.feed_wdt();
        delay(10);
    }
    return true;
}

} // namespace uc8179
} // namespace esphome