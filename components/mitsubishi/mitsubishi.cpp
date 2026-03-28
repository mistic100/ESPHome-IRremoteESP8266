#include "mitsubishi.h"

namespace esphome
{
    namespace mitsubishi
    {
        // copied from ir_Mitsubishi.cpp
        // Mitsubishi A/C
        const uint16_t kMitsubishiAcHdrMark = 3400;
        const uint16_t kMitsubishiAcHdrSpace = 1750;
        const uint16_t kMitsubishiAcBitMark = 450;
        const uint16_t kMitsubishiAcOneSpace = 1300;
        const uint16_t kMitsubishiAcZeroSpace = 420;
        const uint16_t kMitsubishiAcRptMark = 440;
        const uint16_t kMitsubishiAcRptSpace = 15500;

        // Mitsubishi 136 bit A/C
        const uint16_t kMitsubishi136HdrMark = 3324;
        const uint16_t kMitsubishi136HdrSpace = 1474;
        const uint16_t kMitsubishi136BitMark = 467;
        const uint16_t kMitsubishi136OneSpace = 1137;
        const uint16_t kMitsubishi136ZeroSpace = 351;
        const uint32_t kMitsubishi136Gap = kDefaultMessageGap;

        // Mitsubishi 112 bit A/C
        const uint16_t kMitsubishi112HdrMark = 3450;
        const uint16_t kMitsubishi112HdrSpace = 1696;
        const uint16_t kMitsubishi112BitMark = 450;
        const uint16_t kMitsubishi112OneSpace = 1250;
        const uint16_t kMitsubishi112ZeroSpace = 385;
        const uint32_t kMitsubishi112Gap = kDefaultMessageGap;

        static const char *const TAG = "mitsubishi.climate";

        void MitsubishiClimate::set_model(const Model model)
        {
            this->model_ = model;

            switch (this->model_)
            {
            case Model::MITSUBISHI_AC:
                this->minimum_temperature_ = kMitsubishiAcMinTemp;
                this->maximum_temperature_ = kMitsubishiAcMaxTemp;
                this->supports_fan_only_ = true;
                this->fan_modes_ = {
                    climate::CLIMATE_FAN_AUTO,
                    climate::CLIMATE_FAN_LOW,
                    climate::CLIMATE_FAN_MEDIUM,
                    climate::CLIMATE_FAN_HIGH,
                    climate::CLIMATE_FAN_QUIET,
                };
                this->swing_modes_ = {
                    climate::CLIMATE_SWING_OFF,
                    climate::CLIMATE_SWING_VERTICAL,
                    climate::CLIMATE_SWING_HORIZONTAL,
                    climate::CLIMATE_SWING_BOTH
                };
                break;
            case Model::MITSUBISHI136:
                this->minimum_temperature_ = kMitsubishi136MinTemp;
                this->maximum_temperature_ = kMitsubishi136MaxTemp;
                this->supports_fan_only_ = true;
                this->fan_modes_ = {
                    climate::CLIMATE_FAN_LOW,
                    climate::CLIMATE_FAN_MEDIUM,
                    climate::CLIMATE_FAN_HIGH,
                    climate::CLIMATE_FAN_QUIET
                };
                this->swing_modes_ = {
                    climate::CLIMATE_SWING_OFF,
                    climate::CLIMATE_SWING_VERTICAL
                };
                break;
            case Model::MITSUBISHI112:
                this->minimum_temperature_ = kMitsubishi112MinTemp;
                this->maximum_temperature_ = kMitsubishi112MaxTemp;
                this->supports_fan_only_ = false;
                this->fan_modes_ = {
                    climate::CLIMATE_FAN_LOW,
                    climate::CLIMATE_FAN_MEDIUM,
                    climate::CLIMATE_FAN_HIGH,
                    climate::CLIMATE_FAN_QUIET,
                };
                this->swing_modes_ = {
                    climate::CLIMATE_SWING_OFF,
                    climate::CLIMATE_SWING_VERTICAL,
                    climate::CLIMATE_SWING_HORIZONTAL,
                    climate::CLIMATE_SWING_BOTH
                };
                break;
            }
        }

        void MitsubishiClimate::setup()
        {
            climate_ir::ClimateIR::setup();
            this->apply_state();
        }

        void MitsubishiClimate::transmit_state()
        {
            this->apply_state();
            this->send();
        }

        void MitsubishiClimate::send()
        {
            uint8_t *message;

            switch (this->model_)
            {
            case Model::MITSUBISHI_AC:
                message = this->ac_.getRaw();
                sendGeneric(
                    kMitsubishiAcHdrMark, kMitsubishiAcHdrSpace,
                    kMitsubishiAcBitMark, kMitsubishiAcOneSpace,
                    kMitsubishiAcBitMark, kMitsubishiAcZeroSpace,
                    kMitsubishiAcRptMark, kMitsubishiAcRptSpace,
                    message, kMitsubishiACStateLength,
                    38000
                );
                break;
            case Model::MITSUBISHI136:
                message = this->ac_136_.getRaw();
                sendGeneric(
                    kMitsubishi136HdrMark, kMitsubishi136HdrSpace,
                    kMitsubishi136BitMark, kMitsubishi136OneSpace,
                    kMitsubishi136BitMark, kMitsubishi136ZeroSpace,
                    kMitsubishi136BitMark, kMitsubishi136Gap,
                    message, kMitsubishi136StateLength,
                    38000
                );
                break;
            case Model::MITSUBISHI112:
            default:
                message = this->ac_112_.getRaw();
                sendGeneric(
                    kMitsubishi112HdrMark, kMitsubishi112HdrSpace,
                    kMitsubishi112BitMark, kMitsubishi112OneSpace,
                    kMitsubishi112BitMark, kMitsubishi112ZeroSpace,
                    kMitsubishi112BitMark, kMitsubishi112Gap,
                    message, kMitsubishi112StateLength,
                    38000
                );
                break;
            }
        }

        void MitsubishiClimate::apply_state()
        {
            switch (this->model_)
            {
            case Model::MITSUBISHI_AC:
                apply_state_ac();
                break;
            case Model::MITSUBISHI136:
                apply_state_136();
                break;
            case Model::MITSUBISHI112:
                apply_state_112();
                break;
            }
        }

        void MitsubishiClimate::apply_state_ac()
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
                    this->ac_.setMode(kMitsubishiAcAuto);
                    break;
                case climate::CLIMATE_MODE_HEAT:
                    this->ac_.setMode(kMitsubishiAcHeat);
                    break;
                case climate::CLIMATE_MODE_COOL:
                    this->ac_.setMode(kMitsubishiAcCool);
                    break;
                case climate::CLIMATE_MODE_DRY:
                    this->ac_.setMode(kMitsubishiAcDry);
                    break;
                case climate::CLIMATE_MODE_FAN_ONLY:
                    this->ac_.setMode(kMitsubishiAcFan);
                    break;
                }

                if (this->fan_mode.has_value())
                {
                    // 0 is auto, 1-5 is speed, 6 is silent.
                    switch (this->fan_mode.value())
                    {
                    case climate::CLIMATE_FAN_AUTO:
                        this->ac_.setFan(kMitsubishiAcFanAuto);
                        break;
                    case climate::CLIMATE_FAN_QUIET:
                        this->ac_.setFan(kMitsubishiAcFanSilent);
                        break;
                    case climate::CLIMATE_FAN_LOW:
                        this->ac_.setFan(1);
                        break;
                    case climate::CLIMATE_FAN_MEDIUM:
                        this->ac_.setFan(3);
                        break;
                    case climate::CLIMATE_FAN_HIGH:
                        this->ac_.setFan(kMitsubishiAcFanMax);
                        break;
                    }
                }

                switch (this->swing_mode)
                {
                case climate::CLIMATE_SWING_OFF:
                    this->ac_.setVane(kMitsubishiAcVaneMiddle);
                    this->ac_.setWideVane(kMitsubishiAcWideVaneMiddle);
                    break;
                case climate::CLIMATE_SWING_VERTICAL:
                    this->ac_.setVane(kMitsubishiAcVaneSwing);
                    this->ac_.setWideVane(kMitsubishiAcWideVaneMiddle);
                    break;
                case climate::CLIMATE_SWING_HORIZONTAL:
                    this->ac_.setVane(kMitsubishiAcVaneMiddle);
                    this->ac_.setWideVane(kMitsubishiAcWideVaneAuto);
                    break;
                case climate::CLIMATE_SWING_BOTH:
                    this->ac_.setVane(kMitsubishiAcVaneSwing);
                    this->ac_.setWideVane(kMitsubishiAcWideVaneAuto);
                    break;
                }

                this->ac_.on();
            }

            ESP_LOGI(TAG, "%s", this->ac_.toString().c_str());
        }

        void MitsubishiClimate::apply_state_136()
        {
            if (this->mode == climate::CLIMATE_MODE_OFF)
            {
                this->ac_136_.off();
            }
            else
            {
                this->ac_136_.setTemp(this->target_temperature);

                switch (this->mode)
                {
                case climate::CLIMATE_MODE_HEAT_COOL:
                    this->ac_136_.setMode(kMitsubishi136Auto);
                    break;
                case climate::CLIMATE_MODE_HEAT:
                    this->ac_136_.setMode(kMitsubishi136Heat);
                    break;
                case climate::CLIMATE_MODE_COOL:
                    this->ac_136_.setMode(kMitsubishi136Cool);
                    break;
                case climate::CLIMATE_MODE_DRY:
                    this->ac_136_.setMode(kMitsubishi136Dry);
                    break;
                case climate::CLIMATE_MODE_FAN_ONLY:
                    this->ac_136_.setMode(kMitsubishi136Fan);
                    break;
                }

                if (this->fan_mode.has_value())
                {
                    switch (this->fan_mode.value())
                    {
                    case climate::CLIMATE_FAN_QUIET:
                        this->ac_136_.setFan(kMitsubishi136FanMin);
                        break;
                    case climate::CLIMATE_FAN_LOW:
                        this->ac_136_.setFan(kMitsubishi136FanLow);
                        break;
                    case climate::CLIMATE_FAN_MEDIUM:
                        this->ac_136_.setFan(kMitsubishi136FanMed);
                        break;
                    case climate::CLIMATE_FAN_HIGH:
                        this->ac_136_.setFan(kMitsubishi136FanMax);
                        break;
                    }
                }

                switch (this->swing_mode)
                {
                case climate::CLIMATE_SWING_OFF:
                    this->ac_136_.setSwingV(kMitsubishi112SwingVMiddle);
                    break;
                case climate::CLIMATE_SWING_VERTICAL:
                    this->ac_136_.setSwingV(kMitsubishi136SwingVAuto);
                    break;
                }

                this->ac_136_.on();
            }

            ESP_LOGI(TAG, "%s", this->ac_136_.toString().c_str());
        }

        void MitsubishiClimate::apply_state_112()
        {
            if (this->mode == climate::CLIMATE_MODE_OFF)
            {
                this->ac_112_.off();
            }
            else
            {
                this->ac_112_.setTemp(this->target_temperature);

                switch (this->mode)
                {
                case climate::CLIMATE_MODE_HEAT_COOL:
                    this->ac_112_.setMode(kMitsubishi112Auto);
                    break;
                case climate::CLIMATE_MODE_HEAT:
                    this->ac_112_.setMode(kMitsubishi112Heat);
                    break;
                case climate::CLIMATE_MODE_COOL:
                    this->ac_112_.setMode(kMitsubishi112Cool);
                    break;
                case climate::CLIMATE_MODE_DRY:
                    this->ac_112_.setMode(kMitsubishi112Dry);
                    break;
                }

                if (this->fan_mode.has_value())
                {
                    switch (this->fan_mode.value())
                    {
                    case climate::CLIMATE_FAN_QUIET:
                        this->ac_112_.setFan(kMitsubishi112FanMin);
                        break;
                    case climate::CLIMATE_FAN_LOW:
                        this->ac_112_.setFan(kMitsubishi112FanLow);
                        break;
                    case climate::CLIMATE_FAN_MEDIUM:
                        this->ac_112_.setFan(kMitsubishi112FanMed);
                        break;
                    case climate::CLIMATE_FAN_HIGH:
                        this->ac_112_.setFan(kMitsubishi112FanMax);
                        break;
                    }
                }

                switch (this->swing_mode)
                {
                case climate::CLIMATE_SWING_OFF:
                    this->ac_112_.setSwingV(kMitsubishi112SwingVMiddle);
                    this->ac_112_.setSwingH(kMitsubishi112SwingHMiddle);
                    break;
                case climate::CLIMATE_SWING_VERTICAL:
                    this->ac_112_.setSwingV(kMitsubishi112SwingVAuto);
                    this->ac_112_.setSwingH(kMitsubishi112SwingHMiddle);
                    break;
                case climate::CLIMATE_SWING_HORIZONTAL:
                    this->ac_112_.setSwingV(kMitsubishi112SwingVMiddle);
                    this->ac_112_.setSwingH(kMitsubishi112SwingHAuto);
                    break;
                case climate::CLIMATE_SWING_BOTH:
                    this->ac_112_.setSwingV(kMitsubishi112SwingVAuto);
                    this->ac_112_.setSwingH(kMitsubishi112SwingHAuto);
                    break;
                }

                this->ac_112_.on();
            }

            ESP_LOGI(TAG, "%s", this->ac_112_.toString().c_str());
        }

    } // namespace mitsubishi
} // namespace esphome
