#pragma once

#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/climate_ir/climate_ir.h"
#include "esphome/components/ir_remote_base/ir_remote_base.h"
#include "ir_Sharp.h"

namespace esphome
{
    namespace sharp
    {
        enum Model {
            A907 = sharp_ac_remote_model_t::A907,
            A705 = sharp_ac_remote_model_t::A705,
            A903 = sharp_ac_remote_model_t::A903,
        };

        class SharpClimate : public climate_ir::ClimateIR, public ir_remote_base::IrRemoteBase
        {
        public:
            SharpClimate()
                : ClimateIR(kSharpAcMinTemp, kSharpAcMaxTemp, 1.0f, true, true,
                            {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_FAN_HIGH},
                            {climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL}) {}

            void set_model(const Model model);

            void setup() override;

        protected:
            void transmit_state() override;

        private:
            void send();
            void apply_state();

            IRSharpAc ac_ = IRSharpAc(255); // pin is not used
        };

    } // namespace sharp
} // namespace esphome
