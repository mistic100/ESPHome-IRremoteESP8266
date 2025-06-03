#pragma once

#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/climate_ir/climate_ir.h"

#include "ir_Panasonic.h"
#include "esphome/components/ir_remote_base/ir_remote_base.h"

namespace esphome
{
    namespace panasonic
    {
        enum Model {
            LKE = panasonic_ac_remote_model_t::kPanasonicLke,
            NKE = panasonic_ac_remote_model_t::kPanasonicNke,
            DKE = panasonic_ac_remote_model_t::kPanasonicDke,
            JKE = panasonic_ac_remote_model_t::kPanasonicJke,
            CKP = panasonic_ac_remote_model_t::kPanasonicCkp,
            RKR = panasonic_ac_remote_model_t::kPanasonicRkr
        };

        class PanasonicClimate : public climate_ir::ClimateIR, public ir_remote_base::IrRemoteBase
        {
        public:
            PanasonicClimate()
                : ClimateIR(16, 30, 1.0f, true, true,
                            {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_FAN_QUIET},
                            {climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL}) {}

            void set_model(const Model model);
            void setup() override;
            climate::ClimateTraits traits() override;

        protected:
            void transmit_state() override;
            void apply_state();

            IRPanasonicAc ac_ = IRPanasonicAc(255); // pin is not used
        };

    } // namespace panasonic
} // namespace esphome
