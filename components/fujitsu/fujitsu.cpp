#include "fujitsu.h"

namespace esphome
{
    namespace fujitsu
    {
        // copied from ir_Fujitsu.cpp
        const uint16_t kFujitsuAcHdrMark = 3324;
        const uint16_t kFujitsuAcHdrSpace = 1574;
        const uint16_t kFujitsuAcBitMark = 448;
        const uint16_t kFujitsuAcOneSpace = 1182;
        const uint16_t kFujitsuAcZeroSpace = 390;
        const uint16_t kFujitsuAcMinGap = 8100;

        static const char *const TAG = "fujitsu.climate";

        void FujitsuClimate::set_model(const Model model)
        {
            this->ac_.setModel((fujitsu_ac_remote_model_t) model);
        }

        void FujitsuClimate::setup()
        {
            climate_ir::ClimateIR::setup();
            this->apply_state();
        }

        climate::ClimateTraits FujitsuClimate::traits()
        {
            auto traits = climate_ir::ClimateIR::traits();
            if (this->supports_horizontal_swing())
            {
                traits.add_supported_swing_mode(climate::CLIMATE_SWING_HORIZONTAL);
                traits.add_supported_swing_mode(climate::CLIMATE_SWING_BOTH);
            }
            if (this->supports_econo_powerful())
            {
                traits.set_supported_presets({
                    climate::CLIMATE_PRESET_NONE,
                    climate::CLIMATE_PRESET_ECO,
                    climate::CLIMATE_PRESET_BOOST,
                });
            }
            return traits;
        }

        void FujitsuClimate::transmit_state()
        {
            this->apply_state();
            this->send();
        }

        void FujitsuClimate::control(const climate::ClimateCall &call)
        {
            // Intercept preset changes BEFORE delegating to the parent so that
            // we send the toggle command(s) for Eco / Powerful as needed.
            // Parent's control() will then transmit the regular state frame
            // for any other change (mode, temp, fan, swing).
            if (call.get_preset().has_value())
            {
                auto desired_preset = *call.get_preset();
                bool desired_econo = (desired_preset == climate::CLIMATE_PRESET_ECO);
                bool desired_powerful = (desired_preset == climate::CLIMATE_PRESET_BOOST);

                if (desired_econo != this->econo_state_)
                    this->toggle_econo();
                if (desired_powerful != this->powerful_state_)
                    this->toggle_powerful();
            }

            climate_ir::ClimateIR::control(call);
        }

        void FujitsuClimate::step_horizontal()
        {
            if (this->supports_horizontal_swing())
            {
                this->ac_.stepHoriz();
                ESP_LOGI(TAG, "%s", this->ac_.toString().c_str());
                this->send();
            }
            else
            {
                ESP_LOGW(TAG, "Model does not support horizontal swing");
            }
        }

        void FujitsuClimate::step_vertical()
        {
            this->ac_.stepVert();
            ESP_LOGI(TAG, "%s", this->ac_.toString().c_str());
            this->send();
        }

        void FujitsuClimate::toggle_econo()
        {
            if (this->supports_econo_powerful())
            {
                this->ac_.setCmd(kFujitsuAcCmdEcono);
                this->send();
                this->econo_state_ = !this->econo_state_;
                this->powerful_state_ = false;
                ESP_LOGI(TAG, "Toggling Eco (now %s)", this->econo_state_ ? "ON" : "OFF");
                this->sync_preset_to_state();
            }
            else
            {
                ESP_LOGW(TAG, "Model does not support econo mode");
            }
        }

        void FujitsuClimate::toggle_powerful()
        {
            if (this->supports_econo_powerful())
            {
                this->ac_.setCmd(kFujitsuAcCmdPowerful);
                this->send();
                this->powerful_state_ = !this->powerful_state_;
                this->econo_state_ = false;
                ESP_LOGI(TAG, "Toggling Powerful (now %s)", this->powerful_state_ ? "ON" : "OFF");
                this->sync_preset_to_state();
            }
            else
            {
                ESP_LOGW(TAG, "Model does not support powerful mode");
            }
        }

        // Reflect the internal Eco / Powerful tracking into HA's preset field.
        // ECO and BOOST are mutually exclusive in HA's preset model, so when both
        // are internally on we prefer ECO (last-toggled-wins is harder to track
        // and not particularly useful — Eco wins as the more common everyday mode).
        void FujitsuClimate::sync_preset_to_state()
        {
            if (this->econo_state_)
                this->preset = climate::CLIMATE_PRESET_ECO;
            else if (this->powerful_state_)
                this->preset = climate::CLIMATE_PRESET_BOOST;
            else
                this->preset = climate::CLIMATE_PRESET_NONE;
            this->publish_state();
        }

        void FujitsuClimate::send()
        {
            uint8_t *message = this->ac_.getRaw();
            uint8_t length = this->ac_.getStateLength();

            sendGeneric(
                kFujitsuAcHdrMark, kFujitsuAcHdrSpace,
                kFujitsuAcBitMark, kFujitsuAcOneSpace,
                kFujitsuAcBitMark, kFujitsuAcZeroSpace,
                kFujitsuAcBitMark, kFujitsuAcMinGap,
                message, length,
                38000
            );
        }

        void FujitsuClimate::apply_state()
        {
            if (this->mode == climate::CLIMATE_MODE_OFF)
            {
                this->ac_.off();
            }
            else
            {
                this->ac_.setTemp(this->target_temperature);

                switch (this->mode)
                {
                case climate::CLIMATE_MODE_HEAT_COOL:
                    this->ac_.setMode(kFujitsuAcModeAuto);
                    break;
                case climate::CLIMATE_MODE_HEAT:
                    this->ac_.setMode(kFujitsuAcModeHeat);
                    break;
                case climate::CLIMATE_MODE_COOL:
                    this->ac_.setMode(kFujitsuAcModeCool);
                    break;
                case climate::CLIMATE_MODE_DRY:
                    this->ac_.setMode(kFujitsuAcModeDry);
                    break;
                case climate::CLIMATE_MODE_FAN_ONLY:
                    this->ac_.setMode(kFujitsuAcModeFan);
                    break;
                }

                if (this->fan_mode.has_value())
                {
                    switch (this->fan_mode.value())
                    {
                    case climate::CLIMATE_FAN_AUTO:
                        this->ac_.setFanSpeed(kFujitsuAcFanAuto);
                        break;
                    case climate::CLIMATE_FAN_QUIET:
                        this->ac_.setFanSpeed(kFujitsuAcFanQuiet);
                        break;
                    case climate::CLIMATE_FAN_LOW:
                        this->ac_.setFanSpeed(kFujitsuAcFanLow);
                        break;
                    case climate::CLIMATE_FAN_MEDIUM:
                        this->ac_.setFanSpeed(kFujitsuAcFanMed);
                        break;
                    case climate::CLIMATE_FAN_HIGH:
                        this->ac_.setFanSpeed(kFujitsuAcFanHigh);
                        break;
                    }
                }

                switch (this->swing_mode)
                {
                case climate::CLIMATE_SWING_OFF:
                    this->ac_.setSwing(kFujitsuAcSwingOff);
                    break;
                case climate::CLIMATE_SWING_VERTICAL:
                    this->ac_.setSwing(kFujitsuAcSwingVert);
                    break;
                case climate::CLIMATE_SWING_HORIZONTAL:
                    this->ac_.setSwing(kFujitsuAcSwingHoriz);
                    break;
                case climate::CLIMATE_SWING_BOTH:
                    this->ac_.setSwing(kFujitsuAcSwingBoth);
                    break;
                }

                this->ac_.on();
            }

            ESP_LOGI(TAG, "%s", this->ac_.toString().c_str());
        }

    } // namespace fujitsu_general
} // namespace esphome
