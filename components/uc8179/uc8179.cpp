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
}

void UC8179Base::loop()
{
    // Tasks here will be performed at every call of the main application loop.
    // Note: code here MUST NOT BLOCK (see below)
}

void UC8179Base::dump_config()
{
    ESP_LOGCONFIG(TAG, "UC8179 EPD Driver");
    ESP_LOGCONFIG(TAG, "  dc_pin = %s", this->dc_pin_->dump_summary());
    if(this->busy_pin_ != nullptr) {
        ESP_LOGCONFIG(TAG, "  busy_pin = %s", this->busy_pin_->dump_summary());
    }
    if(this->reset_pin_ != nullptr) {
        ESP_LOGCONFIG(TAG, "  reset_pin = %s", this->reset_pin_->dump_summary());
    }
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
        delay(1);
    }
    return true;
}

} // namespace uc8179
} // namespace esphome