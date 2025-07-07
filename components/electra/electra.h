#pragma once

#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/climate_ir/climate_ir.h"
#include "ir_Electra.h"

namespace esphome
{
    namespace electra
    {

        class ElectraClimate : public climate_ir::ClimateIR
        {
        public:
            ElectraClimate()
                : ClimateIR(16, 30, 1.0f, true, true,
                            {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_FAN_HIGH},
                            {climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL, climate::CLIMATE_SWING_HORIZONTAL, climate::CLIMATE_SWING_BOTH}) {}

            void setup() override;

        protected:
            void transmit_state() override;

        private:
            void send();
            void apply_state();

            IRElectraAc ac_ = IRElectraAc(255); // pin is not used
        };

    } // namespace electra
} // namespace esphome
