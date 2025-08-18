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

protected:
    GPIOPin *dc_pin_{nullptr};
    GPIOPin *busy_pin_{nullptr};
    GPIOPin *reset_pin_{nullptr};

    void reset_() {
        if (this->reset_pin_ != nullptr) {
            this->reset_pin_->digital_write(false);
            delay(2); // min 50us
            this->reset_pin_->digital_write(true);
            delay(20);
        }
    }

    void start_command_();
    void end_command_();
    void start_data_();
    void end_data_();
};
class UC8179 : public UC8179Base
{
public:
    void set_idle_timeout(uint32_t idle_timeout) { this->idle_timeout_ = idle_timeout; }


protected:
    bool wait_until_idle_();
    uint32_t idle_timeout_{1000u};
};

} // namespace uc8179
} // namespace esphome