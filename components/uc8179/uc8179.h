#pragma once

#include "esphome/core/component.h"

namespace esphome
{
namespace uc8719
{

class UC8179 : public Component
{
public:
    void setup() override;
    void loop() override;
    void dump_config() override;

    void set_foo(bool foo) { this->foo_ = foo; }
    void set_bar(std::string bar) { this->bar_ = bar; }
    void set_baz(int baz) { this->baz_ = baz; }

protected:
    bool foo_{false};
    std::string bar_{};
    int baz_{0};
};

} // namespace uc8719
} // namespace esphome