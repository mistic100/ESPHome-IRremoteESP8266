#include "haier.h"

namespace esphome
{
    namespace haier
    {
        // copied from ir_Haier.cpp
        const uint16_t kHaierAcHdr = 3000;
        const uint16_t kHaierAcHdrGap = 4300;
        const uint16_t kHaierAcBitMark = 520;
        const uint16_t kHaierAcOneSpace = 1650;
        const uint16_t kHaierAcZeroSpace = 650;
        const uint32_t kHaierAcMinGap = 150000;

        static const char *const TAG = "haier.climate";

        void HaierClimate::set_model(const Model model)
        {
            this->model_ = model;

            if (model == Model::HAIER_AC176 || model == Model::HAIER_AC_YRW02)
            {
                this->swing_modes_ = {
                    climate::CLIMATE_SWING_OFF,
                    climate::CLIMATE_SWING_VERTICAL,
                    climate::CLIMATE_SWING_HORIZONTAL,
                    climate::CLIMATE_SWING_BOTH
                };
            } else {
                this->swing_modes_ = {
                    climate::CLIMATE_SWING_OFF,
                    climate::CLIMATE_SWING_VERTICAL
                };
            }
        }

        void HaierClimate::setup()
        {
            climate_ir::ClimateIR::setup();
            this->apply_state();
        }

        void HaierClimate::transmit_state()
        {
            this->apply_state();
            this->send_debounced();
        }

        void HaierClimate::send()
        {
            uint8_t *message;
            uint16_t nbytes;
            
            switch (this->model_)
            {
                case Model::HAIER_AC:
                    message = this->ac_.getRaw();
                    nbytes = kHaierACStateLength; 
                    break;
                case Model::HAIER_AC160:
                    message = this->ac160_.getRaw();
                    nbytes = kHaierAC160StateLength;
                    break;
                case Model::HAIER_AC176:
                    message = this->ac176_.getRaw();
                    nbytes = kHaierAC176StateLength;
                    break;
                case Model::HAIER_AC_YRW02:
                    message = this->acyrw02_.getRaw();
                    nbytes = kHaierACYRW02StateLength;
                    break;
                default:
                    return;
            }

            this->send_generic(
                kHaierAcHdr, kHaierAcHdrGap,
                kHaierAcBitMark, kHaierAcOneSpace,
                kHaierAcBitMark, kHaierAcZeroSpace,
                kHaierAcBitMark, kHaierAcMinGap,
                message, nbytes,
                38000,
                [](esphome::remote_base::RemoteTransmitData *data) -> void {
                    data->mark(kHaierAcHdr);
                    data->space(kHaierAcHdr);
                }
            );
        }

        void HaierClimate::apply_state()
        {
            switch (this->model_)
            {
            case Model::HAIER_AC:
                apply_state_ac();
                break;
            case Model::HAIER_AC160:
                apply_state_ac160();
                break;
            case Model::HAIER_AC176:
                apply_state_ac176();
                break;
            case Model::HAIER_AC_YRW02:
                apply_state_acyrw02();
                break;
            }
        }

        void HaierClimate::apply_state_ac()
        {
            if (this->mode == climate::CLIMATE_MODE_OFF)
            {
                this->ac_.setCommand(kHaierAcCmdOff);
            }
            else
            {
                this->ac_.setTemp(this->target_temperature);

                switch (this->mode)
                {
                case climate::CLIMATE_MODE_HEAT_COOL:
                    this->ac_.setMode(kHaierAcAuto);
                    break;
                case climate::CLIMATE_MODE_HEAT:
                    this->ac_.setMode(kHaierAcHeat);
                    break;
                case climate::CLIMATE_MODE_COOL:
                    this->ac_.setMode(kHaierAcCool);
                    break;
                case climate::CLIMATE_MODE_DRY:
                    this->ac_.setMode(kHaierAcDry);
                    break;
                case climate::CLIMATE_MODE_FAN_ONLY:
                    this->ac_.setMode(kHaierAcFan);
                    break;
                }

                if (this->fan_mode.has_value())
                {
                    switch (this->fan_mode.value())
                    {
                    case climate::CLIMATE_FAN_AUTO:
                        this->ac_.setFan(kHaierAcFanAuto);
                        break;
                    case climate::CLIMATE_FAN_LOW:
                        this->ac_.setFan(kHaierAcFanLow);
                        break;
                    case climate::CLIMATE_FAN_MEDIUM:
                        this->ac_.setFan(kHaierAcFanMed);
                        break;
                    case climate::CLIMATE_FAN_HIGH:
                        this->ac_.setFan(kHaierAcFanHigh);
                        break;
                    }
                }

                switch (this->swing_mode)
                {
                case climate::CLIMATE_SWING_OFF:
                    this->ac_.setSwingV(kHaierAcSwingVOff);
                    break;
                case climate::CLIMATE_SWING_VERTICAL:
                    this->ac_.setSwingV(kHaierAcSwingVChg);
                    break;
                }

                this->ac_.setCommand(kHaierAcCmdOn);
            }

            ESP_LOGI(TAG, "%s", this->ac_.toString().c_str());
        }

        void HaierClimate::apply_state_ac160()
        {
            if (this->mode == climate::CLIMATE_MODE_OFF)
            {
                this->ac160_.off();
            }
            else
            {
                this->ac160_.setTemp(this->target_temperature);

                switch (this->mode)
                {
                case climate::CLIMATE_MODE_HEAT_COOL:
                    this->ac160_.setMode(kHaierAcYrw02Auto);
                    break;
                case climate::CLIMATE_MODE_HEAT:
                    this->ac160_.setMode(kHaierAcYrw02Heat);
                    break;
                case climate::CLIMATE_MODE_COOL:
                    this->ac160_.setMode(kHaierAcYrw02Cool);
                    break;
                case climate::CLIMATE_MODE_DRY:
                    this->ac160_.setMode(kHaierAcYrw02Dry);
                    break;
                case climate::CLIMATE_MODE_FAN_ONLY:
                    this->ac160_.setMode(kHaierAcYrw02Fan);
                    break;
                }

                if (this->fan_mode.has_value())
                {
                    switch (this->fan_mode.value())
                    {
                    case climate::CLIMATE_FAN_AUTO:
                        this->ac160_.setFan(kHaierAcYrw02FanAuto);
                        break;
                    case climate::CLIMATE_FAN_LOW:
                        this->ac160_.setFan(kHaierAcYrw02FanLow);
                        break;
                    case climate::CLIMATE_FAN_MEDIUM:
                        this->ac160_.setFan(kHaierAcYrw02FanMed);
                        break;
                    case climate::CLIMATE_FAN_HIGH:
                        this->ac160_.setFan(kHaierAcYrw02FanHigh);
                        break;
                    }
                }

                switch (this->swing_mode)
                {
                case climate::CLIMATE_SWING_OFF:
                    this->ac160_.setSwingV(kHaierAc160SwingVOff);
                    break;
                case climate::CLIMATE_SWING_VERTICAL:
                    this->ac160_.setSwingV(kHaierAc160SwingVAuto);
                    break;
                }

                this->ac160_.on();
            }

            ESP_LOGI(TAG, "%s", this->ac160_.toString().c_str());
        }

        void HaierClimate::apply_state_ac176()
        {
            if (this->mode == climate::CLIMATE_MODE_OFF)
            {
                this->ac176_.off();
            }
            else
            {
                this->ac176_.setTemp(this->target_temperature);

                switch (this->mode)
                {
                case climate::CLIMATE_MODE_HEAT_COOL:
                    this->ac176_.setMode(kHaierAcYrw02Auto);
                    break;
                case climate::CLIMATE_MODE_HEAT:
                    this->ac176_.setMode(kHaierAcYrw02Heat);
                    break;
                case climate::CLIMATE_MODE_COOL:
                    this->ac176_.setMode(kHaierAcYrw02Cool);
                    break;
                case climate::CLIMATE_MODE_DRY:
                    this->ac176_.setMode(kHaierAcYrw02Dry);
                    break;
                case climate::CLIMATE_MODE_FAN_ONLY:
                    this->ac176_.setMode(kHaierAcYrw02Fan);
                    break;
                }

                if (this->fan_mode.has_value())
                {
                    switch (this->fan_mode.value())
                    {
                    case climate::CLIMATE_FAN_AUTO:
                        this->ac176_.setFan(kHaierAcYrw02FanAuto);
                        break;
                    case climate::CLIMATE_FAN_LOW:
                        this->ac176_.setFan(kHaierAcYrw02FanLow);
                        break;
                    case climate::CLIMATE_FAN_MEDIUM:
                        this->ac176_.setFan(kHaierAcYrw02FanMed);
                        break;
                    case climate::CLIMATE_FAN_HIGH:
                        this->ac176_.setFan(kHaierAcYrw02FanHigh);
                        break;
                    }
                }

                switch (this->swing_mode)
                {
                case climate::CLIMATE_SWING_OFF:
                    this->ac176_.setSwingV(kHaierAcYrw02SwingVOff);
                    this->ac176_.setSwingH(kHaierAcYrw02SwingHMiddle);
                    break;
                case climate::CLIMATE_SWING_VERTICAL:
                    this->ac176_.setSwingV(kHaierAcYrw02SwingVAuto);
                    this->ac176_.setSwingH(kHaierAcYrw02SwingHMiddle);
                    break;
                case climate::CLIMATE_SWING_HORIZONTAL:
                    this->ac176_.setSwingV(kHaierAcYrw02SwingVOff);
                    this->ac176_.setSwingH(kHaierAcYrw02SwingHAuto);
                    break;
                case climate::CLIMATE_SWING_BOTH:
                    this->ac176_.setSwingV(kHaierAcYrw02SwingVAuto);
                    this->ac176_.setSwingH(kHaierAcYrw02SwingHAuto);
                    break;
                }

                this->ac176_.on();
            }

            ESP_LOGI(TAG, "%s", this->ac176_.toString().c_str());
        }

        void HaierClimate::apply_state_acyrw02()
        {
            if (this->mode == climate::CLIMATE_MODE_OFF)
            {
                this->acyrw02_.off();
            }
            else
            {
                this->acyrw02_.setTemp(this->target_temperature);

                switch (this->mode)
                {
                case climate::CLIMATE_MODE_HEAT_COOL:
                    this->acyrw02_.setMode(kHaierAcYrw02Auto);
                    break;
                case climate::CLIMATE_MODE_HEAT:
                    this->acyrw02_.setMode(kHaierAcYrw02Heat);
                    break;
                case climate::CLIMATE_MODE_COOL:
                    this->acyrw02_.setMode(kHaierAcYrw02Cool);
                    break;
                case climate::CLIMATE_MODE_DRY:
                    this->acyrw02_.setMode(kHaierAcYrw02Dry);
                    break;
                case climate::CLIMATE_MODE_FAN_ONLY:
                    this->acyrw02_.setMode(kHaierAcYrw02Fan);
                    break;
                }

                if (this->fan_mode.has_value())
                {
                    switch (this->fan_mode.value())
                    {
                    case climate::CLIMATE_FAN_AUTO:
                        this->acyrw02_.setFan(kHaierAcYrw02FanAuto);
                        break;
                    case climate::CLIMATE_FAN_LOW:
                        this->acyrw02_.setFan(kHaierAcYrw02FanLow);
                        break;
                    case climate::CLIMATE_FAN_MEDIUM:
                        this->acyrw02_.setFan(kHaierAcYrw02FanMed);
                        break;
                    case climate::CLIMATE_FAN_HIGH:
                        this->acyrw02_.setFan(kHaierAcYrw02FanHigh);
                        break;
                    }
                }

                switch (this->swing_mode)
                {
                case climate::CLIMATE_SWING_OFF:
                    this->acyrw02_.setSwingV(kHaierAcYrw02SwingVOff);
                    this->acyrw02_.setSwingH(kHaierAcYrw02SwingHMiddle);
                    break;
                case climate::CLIMATE_SWING_VERTICAL:
                    this->acyrw02_.setSwingV(kHaierAcYrw02SwingVAuto);
                    this->acyrw02_.setSwingH(kHaierAcYrw02SwingHMiddle);
                    break;
                case climate::CLIMATE_SWING_HORIZONTAL:
                    this->acyrw02_.setSwingV(kHaierAcYrw02SwingVOff);
                    this->acyrw02_.setSwingH(kHaierAcYrw02SwingHAuto);
                    break;
                case climate::CLIMATE_SWING_BOTH:
                    this->acyrw02_.setSwingV(kHaierAcYrw02SwingVAuto);
                    this->acyrw02_.setSwingH(kHaierAcYrw02SwingHAuto);
                    break;
                }

                this->acyrw02_.on();
            }

            ESP_LOGI(TAG, "%s", this->acyrw02_.toString().c_str());
        }

    } // namespace haier_general
} // namespace esphome
