#pragma once

#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/ir_remote_base/ir_remote_base.h"
#include "ir_Mitsubishi.h"

namespace esphome
{
    namespace mitsubishi
    {
        enum Model
        {
            MITSUBISHI_AC,
            MITSUBISHI136,
            MITSUBISHI112,
        };

        class MitsubishiClimate : public ir_remote_base::IrRemoteBase
        {
        public:
            MitsubishiClimate()
                : IrRemoteBase(kMitsubishiAcMinTemp, kMitsubishiAcMaxTemp, 1.0f, true, true,
                               {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_FAN_QUIET},
                               {climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL, climate::CLIMATE_SWING_HORIZONTAL, climate::CLIMATE_SWING_BOTH}) {}

            void set_model(const Model model);
            void setup() override;

        protected:
            void transmit_state() override;

        private:
            void send();
            void apply_state();
            void apply_state_ac();
            void apply_state_136();
            void apply_state_112();

            Model model_ = Model::MITSUBISHI_AC;
            IRMitsubishiAC ac_ = IRMitsubishiAC(255); // pin is not used
            IRMitsubishi136 ac_136_ = IRMitsubishi136(255);
            IRMitsubishi112 ac_112_ = IRMitsubishi112(255);
        };

    } // namespace mitsubishi
} // namespace esphome
