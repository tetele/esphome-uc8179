#include "esphome/core/log.h"
#include "uc8179.h"

namespace esphome
{
namespace uc8179
{

static const char *TAG = "uc8179.component";

void UC8179::setup()
{
    // Code here should perform all component initialization,
    //  whether hardware, memory, or otherwise
}

void UC8179::loop()
{
    // Tasks here will be performed at every call of the main application loop.
    // Note: code here MUST NOT BLOCK (see below)
}

void UC8179::dump_config()
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

} // namespace uc8179
} // namespace esphome