#pragma once

#include "display.h"

namespace esphome
{
namespace uc8179
{

class GDEY075T7 : public UC8179Display_G4 {
public:
    void dump_config() override;
    int get_width_internal() override { return 800; }
    int get_height_internal() override { return 480; }
    void setup() override;

    void initialize() override;
};

class GDEY075T7_BW : public UC8179Display_KW {
public:
    void dump_config() override;
    int get_width_internal() override { return 800; }
    int get_height_internal() override { return 480; }
    void setup() override;
};

} // namespace uc8179
} // namespace esphome