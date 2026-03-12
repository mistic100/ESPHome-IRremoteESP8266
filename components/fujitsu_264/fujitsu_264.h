#pragma once

#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/ir_remote_base/ir_remote_base.h"
#include "ir_Fujitsu.h"

namespace esphome
{
    namespace fujitsu_264
    {
        class Fujitsu264Climate : public ir_remote_base::IrRemoteBase
        {
        public:
            Fujitsu264Climate()
                : IrRemoteBase(kFujitsuAc264MinTemp, kFujitsuAc264MaxTemp, 0.5f, true, true,
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
