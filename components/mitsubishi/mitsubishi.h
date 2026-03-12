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
        // Model enum for selecting which Mitsubishi protocol to use
        // GENERAL: IRMitsubishiAC (144-bit) - Most common Mitsubishi Electric units
        //          Compatible: MSZ-GV2519, MSZ-SF25VE, MSZ-FHnnVE, MLZ-RX5017AS, etc.
        // MSY:     IRMitsubishi136 (136-bit) - Ducted systems
        //          Compatible: PEAD-RP71JAA
        // MSH:     IRMitsubishi112 (112-bit) - Specific wall units
        //          Compatible: MSH-A24WV, MUH-A24WV
        enum Model
        {
            GENERAL,
            MSY,
            MSH,
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
            climate::ClimateTraits traits() override;

        protected:
            void transmit_state() override;

        private:
            void send();
            void apply_state();
            void apply_state_general();
            void apply_state_msy();
            void apply_state_msh();

            Model model_ = Model::GENERAL;
            IRMitsubishiAC ac_general_ = IRMitsubishiAC(255);
            IRMitsubishi136 ac_msy_ = IRMitsubishi136(255);
            IRMitsubishi112 ac_msh_ = IRMitsubishi112(255);
        };

    } // namespace mitsubishi
} // namespace esphome
