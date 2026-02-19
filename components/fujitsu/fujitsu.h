#pragma once

#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/climate_ir/climate_ir.h"
#include "esphome/components/ir_remote_base/ir_remote_base.h"
#include "ir_Fujitsu.h"

namespace esphome
{
    namespace fujitsu
    {
        enum Model {
            ARRAH2E = fujitsu_ac_remote_model_t::ARRAH2E,
            ARDB1 = fujitsu_ac_remote_model_t::ARDB1,
            ARREB1E = fujitsu_ac_remote_model_t::ARREB1E,
            ARJW2 = fujitsu_ac_remote_model_t::ARJW2,
            ARRY4 = fujitsu_ac_remote_model_t::ARRY4,
            ARREW4E = fujitsu_ac_remote_model_t::ARREW4E
        };

        class FujitsuClimate : public climate_ir::ClimateIR, public ir_remote_base::IrRemoteBase
        {
        public:
            FujitsuClimate()
                : ClimateIR(kFujitsuAcMinTemp, kFujitsuAcMaxTemp, 1.0f, true, true,
                            {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_FAN_QUIET},
                            {climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL}) {}

            void set_model(const Model model);
            
            void setup() override;
            climate::ClimateTraits traits() override;

            void step_horizontal();
            void step_vertical();

        protected:
            void transmit_state() override;

        private:
            void send();
            void apply_state();

            IRFujitsuAC ac_ = IRFujitsuAC(255); // pin is not used
        };

    } // namespace fujitsu
} // namespace esphome
