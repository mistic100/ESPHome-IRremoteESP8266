#pragma once

#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/climate_ir/climate_ir.h"

#include "ir_Fujitsu.h"
#include "esphome/components/ir_remote_base/ir_remote_base.h"

namespace esphome
{
    namespace fujitsu_264
    {
        class Fujitsu264Climate : public climate_ir::ClimateIR, public ir_remote_base::IrRemoteBase
        {
        public:
            Fujitsu264Climate()
                : ClimateIR(kFujitsuAc264MinTemp, kFujitsuAc264MaxTemp, 0.5f, /* supports_dry */ true, /* supports_fan_only */ true,
                            {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_FAN_QUIET},
                            {climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL}) {}

            void setup() override;

            void set_fan_angle(const uint8_t fan_angle);
            void toggle_powerful();

        protected:
            void transmit_state() override;

        private:
            void send();
            void apply_state();

            IRFujitsuAC264 ac_ = IRFujitsuAC264(255); // pin is not used
        };
    } // namespace fujitsu_264
} // namespace esphome
