#include "mitsubishi.h"

namespace esphome
{
    namespace mitsubishi
    {
        // Timing constants for IRMitsubishiAC (144-bit / GENERAL)
        const uint16_t kMitsubishiAcHdrMark = 3400;
        const uint16_t kMitsubishiAcHdrSpace = 1750;
        const uint16_t kMitsubishiAcBitMark = 450;
        const uint16_t kMitsubishiAcOneSpace = 1300;
        const uint16_t kMitsubishiAcZeroSpace = 420;
        const uint32_t kMitsubishiAcRptMark = 440;
        const uint32_t kMitsubishiAcRptSpace = 17100;

        // Timing constants for IRMitsubishi136 (136-bit / MSY)
        const uint16_t kMitsubishi136HdrMark = 3324;
        const uint16_t kMitsubishi136HdrSpace = 1474;
        const uint16_t kMitsubishi136BitMark = 467;
        const uint16_t kMitsubishi136OneSpace = 1137;
        const uint16_t kMitsubishi136ZeroSpace = 351;
        const uint32_t kMitsubishi136Gap = kDefaultMessageGap;

        // Timing constants for IRMitsubishi112 (112-bit / MSH)
        const uint16_t kMitsubishi112HdrMark = 3450;
        const uint16_t kMitsubishi112HdrSpace = 1696;
        const uint16_t kMitsubishi112BitMark = 450;
        const uint16_t kMitsubishi112OneSpace = 1250;
        const uint16_t kMitsubishi112ZeroSpace = 385;
        const uint32_t kMitsubishi112Gap = kDefaultMessageGap;

        // Common frequency for all Mitsubishi AC protocols
        const uint16_t kMitsubishiAcFreq = 38000;

        static const char *const TAG = "mitsubishi.climate";

        void MitsubishiClimate::set_model(const Model model)
        {
            this->model_ = model;
        }

        void MitsubishiClimate::setup()
        {
            climate_ir::ClimateIR::setup();
            this->apply_state();
        }

        climate::ClimateTraits MitsubishiClimate::traits()
        {
            auto traits = climate_ir::ClimateIR::traits();

            switch (this->model_)
            {
            case Model::GENERAL:
                // GENERAL (144-bit) supports horizontal swing via WideVane
                // All swing modes are supported
                break;
            case Model::MSY:
                // MSY (136-bit) only supports vertical swing, no horizontal
                traits.set_supported_swing_modes({
                    climate::CLIMATE_SWING_OFF,
                    climate::CLIMATE_SWING_VERTICAL,
                });
                // MSY has temp range 17-30
                traits.set_visual_min_temperature(17.0f);
                traits.set_visual_max_temperature(30.0f);
                break;
            case Model::MSH:
                // MSH (112-bit) supports both vertical and horizontal swing
                // All swing modes are supported
                break;
            }

            return traits;
        }

        void MitsubishiClimate::transmit_state()
        {
            this->apply_state();
            this->send();
        }

        void MitsubishiClimate::send()
        {
            uint8_t *message;
            uint16_t state_length;
            uint16_t hdr_mark, hdr_space, bit_mark, one_space, zero_space;
            uint32_t gap;

            switch (this->model_)
            {
            case Model::GENERAL:
                message = this->ac_general_.getRaw();
                state_length = kMitsubishiACStateLength;
                hdr_mark = kMitsubishiAcHdrMark;
                hdr_space = kMitsubishiAcHdrSpace;
                bit_mark = kMitsubishiAcBitMark;
                one_space = kMitsubishiAcOneSpace;
                zero_space = kMitsubishiAcZeroSpace;
                gap = kMitsubishiAcRptSpace;
                break;
            case Model::MSY:
                message = this->ac_msy_.getRaw();
                state_length = kMitsubishi136StateLength;
                hdr_mark = kMitsubishi136HdrMark;
                hdr_space = kMitsubishi136HdrSpace;
                bit_mark = kMitsubishi136BitMark;
                one_space = kMitsubishi136OneSpace;
                zero_space = kMitsubishi136ZeroSpace;
                gap = kMitsubishi136Gap;
                break;
            case Model::MSH:
            default:
                message = this->ac_msh_.getRaw();
                state_length = kMitsubishi112StateLength;
                hdr_mark = kMitsubishi112HdrMark;
                hdr_space = kMitsubishi112HdrSpace;
                bit_mark = kMitsubishi112BitMark;
                one_space = kMitsubishi112OneSpace;
                zero_space = kMitsubishi112ZeroSpace;
                gap = kMitsubishi112Gap;
                break;
            }

            sendGeneric(
                hdr_mark, hdr_space,
                bit_mark, one_space,
                bit_mark, zero_space,
                bit_mark, gap,
                message, state_length,
                kMitsubishiAcFreq);
        }

        void MitsubishiClimate::apply_state()
        {
            switch (this->model_)
            {
            case Model::GENERAL:
                apply_state_general();
                break;
            case Model::MSY:
                apply_state_msy();
                break;
            case Model::MSH:
                apply_state_msh();
                break;
            }
        }

        void MitsubishiClimate::apply_state_general()
        {
            if (this->mode == climate::CLIMATE_MODE_OFF)
            {
                this->ac_general_.off();
            }
            else
            {
                this->ac_general_.setTemp(this->target_temperature);

                switch (this->mode)
                {
                case climate::CLIMATE_MODE_HEAT_COOL:
                    this->ac_general_.setMode(kMitsubishiAcAuto);
                    break;
                case climate::CLIMATE_MODE_HEAT:
                    this->ac_general_.setMode(kMitsubishiAcHeat);
                    break;
                case climate::CLIMATE_MODE_COOL:
                    this->ac_general_.setMode(kMitsubishiAcCool);
                    break;
                case climate::CLIMATE_MODE_DRY:
                    this->ac_general_.setMode(kMitsubishiAcDry);
                    break;
                case climate::CLIMATE_MODE_FAN_ONLY:
                    this->ac_general_.setMode(kMitsubishiAcFan);
                    break;
                }

                if (this->fan_mode.has_value())
                {
                    // IRMitsubishiAC fan speeds: Auto=0, 1-5 (low to high), Silent=6
                    switch (this->fan_mode.value())
                    {
                    case climate::CLIMATE_FAN_AUTO:
                        this->ac_general_.setFan(kMitsubishiAcFanAuto);
                        break;
                    case climate::CLIMATE_FAN_QUIET:
                        this->ac_general_.setFan(kMitsubishiAcFanSilent);
                        break;
                    case climate::CLIMATE_FAN_LOW:
                        this->ac_general_.setFan(1);  // Fan speed 1
                        break;
                    case climate::CLIMATE_FAN_MEDIUM:
                        this->ac_general_.setFan(3);  // Fan speed 3 (middle)
                        break;
                    case climate::CLIMATE_FAN_HIGH:
                        this->ac_general_.setFan(kMitsubishiAcFanMax);
                        break;
                    }
                }

                // Vertical swing via Vane
                switch (this->swing_mode)
                {
                case climate::CLIMATE_SWING_OFF:
                    this->ac_general_.setVane(kMitsubishiAcVaneAuto);
                    this->ac_general_.setWideVane(kMitsubishiAcWideVaneMiddle);
                    break;
                case climate::CLIMATE_SWING_VERTICAL:
                    this->ac_general_.setVane(kMitsubishiAcVaneSwing);
                    this->ac_general_.setWideVane(kMitsubishiAcWideVaneMiddle);
                    break;
                case climate::CLIMATE_SWING_HORIZONTAL:
                    this->ac_general_.setVane(kMitsubishiAcVaneAuto);
                    this->ac_general_.setWideVane(kMitsubishiAcWideVaneAuto);
                    break;
                case climate::CLIMATE_SWING_BOTH:
                    this->ac_general_.setVane(kMitsubishiAcVaneSwing);
                    this->ac_general_.setWideVane(kMitsubishiAcWideVaneAuto);
                    break;
                }

                this->ac_general_.on();
            }

            ESP_LOGI(TAG, "%s", this->ac_general_.toString().c_str());
        }

        void MitsubishiClimate::apply_state_msy()
        {
            if (this->mode == climate::CLIMATE_MODE_OFF)
            {
                this->ac_msy_.off();
            }
            else
            {
                this->ac_msy_.setTemp(this->target_temperature);

                switch (this->mode)
                {
                case climate::CLIMATE_MODE_HEAT_COOL:
                    this->ac_msy_.setMode(kMitsubishi136Auto);
                    break;
                case climate::CLIMATE_MODE_HEAT:
                    this->ac_msy_.setMode(kMitsubishi136Heat);
                    break;
                case climate::CLIMATE_MODE_COOL:
                    this->ac_msy_.setMode(kMitsubishi136Cool);
                    break;
                case climate::CLIMATE_MODE_DRY:
                    this->ac_msy_.setMode(kMitsubishi136Dry);
                    break;
                case climate::CLIMATE_MODE_FAN_ONLY:
                    this->ac_msy_.setMode(kMitsubishi136Fan);
                    break;
                }

                if (this->fan_mode.has_value())
                {
                    switch (this->fan_mode.value())
                    {
                    case climate::CLIMATE_FAN_AUTO:
                        // Mitsubishi136 doesn't have auto fan, use max
                        this->ac_msy_.setFan(kMitsubishi136FanMax);
                        break;
                    case climate::CLIMATE_FAN_QUIET:
                        this->ac_msy_.setFan(kMitsubishi136FanQuiet);
                        break;
                    case climate::CLIMATE_FAN_LOW:
                        this->ac_msy_.setFan(kMitsubishi136FanLow);
                        break;
                    case climate::CLIMATE_FAN_MEDIUM:
                        this->ac_msy_.setFan(kMitsubishi136FanMed);
                        break;
                    case climate::CLIMATE_FAN_HIGH:
                        this->ac_msy_.setFan(kMitsubishi136FanMax);
                        break;
                    }
                }

                // Vertical swing only for MSY
                switch (this->swing_mode)
                {
                case climate::CLIMATE_SWING_OFF:
                    this->ac_msy_.setSwingV(kMitsubishi136SwingVHighest);
                    break;
                case climate::CLIMATE_SWING_VERTICAL:
                    this->ac_msy_.setSwingV(kMitsubishi136SwingVAuto);
                    break;
                default:
                    // MSY doesn't support horizontal swing
                    this->ac_msy_.setSwingV(kMitsubishi136SwingVAuto);
                    break;
                }

                this->ac_msy_.on();
            }

            ESP_LOGI(TAG, "%s", this->ac_msy_.toString().c_str());
        }

        void MitsubishiClimate::apply_state_msh()
        {
            if (this->mode == climate::CLIMATE_MODE_OFF)
            {
                this->ac_msh_.off();
            }
            else
            {
                this->ac_msh_.setTemp(this->target_temperature);

                switch (this->mode)
                {
                case climate::CLIMATE_MODE_HEAT_COOL:
                    this->ac_msh_.setMode(kMitsubishi112Auto);
                    break;
                case climate::CLIMATE_MODE_HEAT:
                    this->ac_msh_.setMode(kMitsubishi112Heat);
                    break;
                case climate::CLIMATE_MODE_COOL:
                    this->ac_msh_.setMode(kMitsubishi112Cool);
                    break;
                case climate::CLIMATE_MODE_DRY:
                    this->ac_msh_.setMode(kMitsubishi112Dry);
                    break;
                case climate::CLIMATE_MODE_FAN_ONLY:
                    // Mitsubishi112 doesn't have a dedicated fan mode, use dry with low fan
                    this->ac_msh_.setMode(kMitsubishi112Dry);
                    break;
                }

                if (this->fan_mode.has_value())
                {
                    switch (this->fan_mode.value())
                    {
                    case climate::CLIMATE_FAN_AUTO:
                        // Mitsubishi112 doesn't have auto fan
                        this->ac_msh_.setFan(kMitsubishi112FanMax);
                        break;
                    case climate::CLIMATE_FAN_QUIET:
                        this->ac_msh_.setFan(kMitsubishi112FanQuiet);
                        break;
                    case climate::CLIMATE_FAN_LOW:
                        this->ac_msh_.setFan(kMitsubishi112FanLow);
                        break;
                    case climate::CLIMATE_FAN_MEDIUM:
                        this->ac_msh_.setFan(kMitsubishi112FanMed);
                        break;
                    case climate::CLIMATE_FAN_HIGH:
                        this->ac_msh_.setFan(kMitsubishi112FanMax);
                        break;
                    }
                }

                // MSH supports both vertical and horizontal swing
                switch (this->swing_mode)
                {
                case climate::CLIMATE_SWING_OFF:
                    this->ac_msh_.setSwingV(kMitsubishi112SwingVMiddle);
                    this->ac_msh_.setSwingH(kMitsubishi112SwingHMiddle);
                    break;
                case climate::CLIMATE_SWING_VERTICAL:
                    this->ac_msh_.setSwingV(kMitsubishi112SwingVAuto);
                    this->ac_msh_.setSwingH(kMitsubishi112SwingHMiddle);
                    break;
                case climate::CLIMATE_SWING_HORIZONTAL:
                    this->ac_msh_.setSwingV(kMitsubishi112SwingVMiddle);
                    this->ac_msh_.setSwingH(kMitsubishi112SwingHAuto);
                    break;
                case climate::CLIMATE_SWING_BOTH:
                    this->ac_msh_.setSwingV(kMitsubishi112SwingVAuto);
                    this->ac_msh_.setSwingH(kMitsubishi112SwingHAuto);
                    break;
                }

                this->ac_msh_.on();
            }

            ESP_LOGI(TAG, "%s", this->ac_msh_.toString().c_str());
        }

    } // namespace mitsubishi
} // namespace esphome
