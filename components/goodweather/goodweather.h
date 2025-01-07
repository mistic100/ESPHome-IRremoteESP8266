#pragma once

#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/climate_ir/climate_ir.h"
#include "ir_Goodweather.h"

namespace esphome
{
    namespace goodweather
    {
        class GoodweatherClimate : public climate_ir::ClimateIR
        {
        public:
            GoodweatherClimate()
                : ClimateIR(16, 31, 1.0f, true, true,
                            {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_FAN_HIGH},
                            {climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL}) {}

            void setup() override;
            climate::ClimateTraits traits() override;

        protected:
            void transmit_state() override;
            void apply_state();

            IRGoodweatherAc ac_ = IRGoodweatherAc(255); // pin is not used
        };

    } // namespace goodweather
} // namespace esphome
