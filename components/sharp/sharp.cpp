#include "sharp.h"

namespace esphome
{
    namespace sharp
    {
        static const char *const TAG = "sharp.climate";

        void SharpClimate::set_model(const Model model)
        {
            this->ac_.setModel((sharp_ac_remote_model_t) model);
        }

        void SharpClimate::setup()
        {
            climate_ir::ClimateIR::setup();
            this->apply_state();
        }

        void SharpClimate::transmit_state()
        {
            this->apply_state();
            this->send();
        }

        void SharpClimate::send()
        {
            uint8_t *message = this->ac_.getRaw();

            sendGeneric(
                kSharpAcHdrMark, kSharpAcHdrSpace,
                kSharpAcBitMark, kSharpAcOneSpace,
                kSharpAcBitMark, kSharpAcZeroSpace,
                kSharpAcBitMark, kSharpAcGap,
                message, kSharpAcStateLength,
                38000
            );
        }

        void SharpClimate::apply_state()
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
                case climate::CLIMATE_MODE_AUTO:
                    this->ac_.setMode(kSharpAcAuto);
                    break;
                case climate::CLIMATE_MODE_HEAT:
                    this->ac_.setMode(kSharpAcHeat);
                    break;
                case climate::CLIMATE_MODE_COOL:
                    this->ac_.setMode(kSharpAcCool);
                    break;
                case climate::CLIMATE_MODE_DRY:
                    this->ac_.setMode(kSharpAcDry);
                    break;
                case climate::CLIMATE_MODE_FAN_ONLY:
                    this->ac_.setMode(kSharpAcFan);
                    break;
                }

                if (this->fan_mode.has_value())
                {
                    switch (this->fan_mode.value())
                    {
                    case climate::CLIMATE_FAN_AUTO:
                        this->ac_.setFan(kSharpAcFanAuto);
                        break;
                    case climate::CLIMATE_FAN_LOW:
                        this->ac_.setFan(kSharpAcFanMin);
                        break;
                    case climate::CLIMATE_FAN_MEDIUM:
                        this->ac_.setFan(kSharpAcFanMed);
                        break;
                    case climate::CLIMATE_FAN_HIGH:
                        this->ac_.setFan(kSharpAcFanHigh);
                        break;
                    }
                }

                switch (this->swing_mode)
                {
                case climate::CLIMATE_SWING_OFF:
                    this->ac_.setSwingV(kSharpAcSwingVOff);
                    break;
                case climate::CLIMATE_SWING_VERTICAL:
                    this->ac_.setSwingV(kSharpAcSwingVLowest);
                    break;
                }

                this->ac_.on();
            }

            ESP_LOGI(TAG, "%s", this->ac_.toString().c_str());
        }
    } // namespace sharp
} // namespace esphome
