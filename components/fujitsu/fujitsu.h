#pragma once

#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/ir_remote_base/ir_remote_base.h"
#include "ir_Fujitsu.h"

namespace esphome
{
    namespace fujitsu
    {
        enum Model
        {
            ARRAH2E = fujitsu_ac_remote_model_t::ARRAH2E,
            ARDB1 = fujitsu_ac_remote_model_t::ARDB1,
            ARREB1E = fujitsu_ac_remote_model_t::ARREB1E,
            ARJW2 = fujitsu_ac_remote_model_t::ARJW2,
            ARRY4 = fujitsu_ac_remote_model_t::ARRY4,
            ARREW4E = fujitsu_ac_remote_model_t::ARREW4E
        };

        class FujitsuClimate : public ir_remote_base::IrRemoteBase
        {
        public:
            FujitsuClimate()
                : IrRemoteBase(kFujitsuAcMinTemp, kFujitsuAcMaxTemp, 1.0f, true, true,
                               {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_FAN_QUIET},
                               {climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL}) {}

            void set_model(const Model model);

            void setup() override;
            climate::ClimateTraits traits() override;

            void step_horizontal();
            void step_vertical();

            // Toggle Economy (Eco) mode. Sends a kFujitsuAcCmdEcono command frame.
            // Internally tracks the assumed Eco state, used by the ECO preset.
            // External toggles via the physical IR remote will desync this state
            // until the next ESPHome-initiated mode/temp/fan/swing change.
            void econo();
            bool is_econo() const { return this->econo_state_; }

            // Toggle Powerful (boost) mode. Same caveats as econo() apply.
            void powerful();
            bool is_powerful() const { return this->powerful_state_; }

        protected:
            void transmit_state() override;
            void control(const climate::ClimateCall &call) override;

        private:
            void send();
            void apply_state();
            void sync_preset_to_state();

            IRFujitsuAC ac_ = IRFujitsuAC(255); // pin is not used

            // Best-effort tracking of toggle-only state bits (Eco / Powerful).
            // Both default to false, matching a fresh power-on of the AC.
            bool econo_state_ = false;
            bool powerful_state_ = false;
        };

    } // namespace fujitsu
} // namespace esphome
